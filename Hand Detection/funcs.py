##############################################################################
#                                                                            #
# Copyright (c) Borja García Quiroga, All Rights Reserved.                   #
#                                                                            #
# The information and material provided below was developed as complementary #
# materials to the dissertation submitted as partial requirements for the    #
# MSc in Computer Science at Trinity College Dublin, Ireland.                #
#                                                                            #
##############################################################################

import numpy as np
import cv2
import torch
import torch.nn as nn
import matplotlib.pyplot as plt
from variables import CVT_IMG_SIZE, ORG_IMG_SIZE, NUM_OF_KEYPOINTS

COLORMAP = {
    "thumb": {"ids": [0, 1, 2, 3, 4], "color": "g"},
    "index": {"ids": [0, 5, 6, 7, 8], "color": "c"},
    "middle": {"ids": [0, 9, 10, 11, 12], "color": "b"},
    "ring": {"ids": [0, 13, 14, 15, 16], "color": "m"},
    "little": {"ids": [0, 17, 18, 19, 20], "color": "r"},
}

def blur_heatmaps(heatmaps):

    '''
    Blurs heatmaps using GaussinaBlur of defined size.
    '''

    heatmaps_blurred = heatmaps.copy()

    for k in range(len(heatmaps)):

        if heatmaps_blurred[k].max() == 1:

            heatmaps_blurred[k] = cv2.GaussianBlur(heatmaps[k], (51, 51), 3)
            heatmaps_blurred[k] = heatmaps_blurred[k] / heatmaps_blurred[k].max()

    return heatmaps_blurred

def display_predictions(batch_data, model):
    
    '''
    Displays the predictions.
    '''

    inputs = batch_data["image"]
    grount_truth = batch_data["keypoints"].numpy()

    batch_size = grount_truth.shape[0]

    pred_heatmaps = model(inputs)
    pred_heatmaps = pred_heatmaps.detach().numpy()
    pred_keypoints = heatmaps_to_coordinates(pred_heatmaps)

    images = batch_data["image_raw"].numpy()
    images = np.moveaxis(images, 1, -1)

    plt.figure(figsize=[12, 4 * batch_size])

    for i in range(batch_size):

        image = images[i]
        grount_truth_img = grount_truth[i] * ORG_IMG_SIZE
        pred_keypoints_img = pred_keypoints[i] * ORG_IMG_SIZE

        plt.subplot(batch_size, 3, i * 3 + 1)
        plt.imshow(image)
        plt.title("Image")
        plt.axis("off")

        plt.subplot(batch_size, 3, i * 3 + 2)
        plt.imshow(image)
        for finger, params in COLORMAP.items():
            plt.plot(
                grount_truth_img[params["ids"], 0],
                grount_truth_img[params["ids"], 1],
                params["color"],
            )
        plt.title("True Keypoints")
        plt.axis("off")

        plt.subplot(batch_size, 3, i * 3 + 3)
        plt.imshow(image)

        for finger, params in COLORMAP.items():

            plt.plot(
                pred_keypoints_img[params["ids"], 0],
                pred_keypoints_img[params["ids"], 1],
                params["color"],
            )

        plt.title("Pred Keypoints")
        plt.axis("off")

    plt.tight_layout()

def project_points(pnts, mat):

    '''
    Projects the points using a projection matrix mat.

    Params:
        pnts: An array of points.
        mat: Projection matrix.

    Returns:
        The points in image space.
    '''

    pnts = np.array(pnts)
    mat = np.array(mat)

    # Transform to image space.
    uv = np.matmul(mat, pnts.T).T

    return uv[:, :2] / uv[:, -1:]

def heatmaps_to_coordinates(heatmaps):

    '''
    Heatmaps is a numpy array.
    '''

    batch_size = heatmaps.shape[0]

    sums = heatmaps.sum(axis=-1).sum(axis=-1)
    sums = np.expand_dims(sums, [2, 3])

    normalized = heatmaps / sums

    x_prob = normalized.sum(axis=2)
    y_prob = normalized.sum(axis=3)

    arr = np.tile(np.float32(np.arange(0, CVT_IMG_SIZE)), [batch_size, 21, 1])

    x = (arr * x_prob).sum(axis=2)
    y = (arr * y_prob).sum(axis=2)

    keypoints = np.stack([x, y], axis=-1)

    return keypoints / CVT_IMG_SIZE

def array_to_heatmaps(keypoints):
    
    '''
    Creates 2D heatmaps from keypoint locations for a single image
    
    Params:
        keypoints: array of size NUM_OF_KEYPOINTS x 2

    Returns:
        An array of size NUM_OF_KEYPOINTS x CVT_IMG_SIZE x CVT_IMG_SIZE
    '''

    heatmaps = np.zeros([NUM_OF_KEYPOINTS, CVT_IMG_SIZE, CVT_IMG_SIZE])

    for k, (x, y) in enumerate(keypoints):

        x, y = int(x * CVT_IMG_SIZE), int(y * CVT_IMG_SIZE)

        if (0 <= x < CVT_IMG_SIZE) and (0 <= y < CVT_IMG_SIZE):

            heatmaps[k, int(y), int(x)] = 1

    heatmaps = blur_heatmaps(heatmaps)

    return heatmaps

def show_data(dataset, n_samples=12):

    '''
    Function to visualize data
    Input: torch.utils.data.Dataset
    '''

    n_cols = 4
    n_rows = int(np.ceil(n_samples / n_cols))
    plt.figure(figsize=[15, n_rows * 4])

    ids = np.random.choice(dataset.__len__(), n_samples, replace=False)

    for i, id_ in enumerate(ids, 1):
        sample = dataset.__getitem__(id_)

        image = sample["image_raw"].numpy()
        image = np.moveaxis(image, 0, -1)
        keypoints = sample["keypoints"].numpy()
        keypoints = keypoints * ORG_IMG_SIZE

        plt.subplot(n_rows, n_cols, i)
        plt.imshow(image)
        plt.scatter(keypoints[:, 0], keypoints[:, 1], c="k", alpha=0.5)

        for finger, params in COLORMAP.items():
            plt.plot(
                keypoints[params["ids"], 0],
                keypoints[params["ids"], 1],
                params["color"],
            )

    plt.tight_layout()
    plt.show()

class IoULoss(nn.Module):

    def __init__(self):

        super(IoULoss, self).__init__()
        self.EPSILON = 1e-6

    def _op_sum(self, x):

        return x.sum(-1).sum(-1)

    def forward(self, y_pred, y_true):

        inter = self._op_sum(y_true * y_pred)
        union = (
            self._op_sum(y_true ** 2)
            + self._op_sum(y_pred ** 2)
            - self._op_sum(y_true * y_pred)
        )

        iou = (inter + self.EPSILON) / (union + self.EPSILON)
        iou = torch.mean(iou)

        return 1 - iou

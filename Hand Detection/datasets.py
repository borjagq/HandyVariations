##############################################################################
#                                                                            #
# Copyright (c) Borja García Quiroga, All Rights Reserved.                   #
#                                                                            #
# The information and material provided below was developed as complementary #
# materials to the dissertation submitted as partial requirements for the    #
# MSc in Computer Science at Trinity College Dublin, Ireland.                #
#                                                                            #
##############################################################################

import os, json
import numpy as np
import torch
from torch.utils.data import Dataset
from torchvision import transforms
from PIL import Image
from funcs import array_to_heatmaps, project_points

from variables import ORG_IMG_SIZE, CVT_IMG_SIZE, DATASET_MEAN, DATASET_STD

class FreiHand(Dataset):

    '''
    Class to load FreiHAND dataset. Augmented images are located
    after the first 32,560 images.

    See: https://lmb.informatik.uni-freiburg.de/projects/freihand/
    '''

    def __init__(self, config, dataset_type="train"):

        '''
        Create a new instance.
        '''

        self.device = config["device"]
        self.dataset_path = os.path.join(config["data_dir"], "training/rgb")
        self.dataset_names = np.sort(os.listdir(self.dataset_path))

        mat = os.path.join(config["data_dir"], "training_K.json")
        with open(mat, "r") as f:
            self.matrix = np.array(json.load(f))

        annotation_fn = os.path.join(config["data_dir"], "training_xyz.json")
        with open(annotation_fn, "r") as f:
            self.annotation = np.array(json.load(f))

        if dataset_type == "train":
            ind_start = 0
            ind_end = 26000

        elif dataset_type == "val":
            ind_start = 26000
            ind_end = 31000

        else:
            ind_start = len(self.annotation) - 1560
            ind_end = len(self.annotation)

            #ind_start = 0
            #ind_end = 1560

        self.dataset_names = self.dataset_names[ind_start:ind_end]
        self.matrix = self.matrix[ind_start:ind_end]
        self.annotation = self.annotation[ind_start:ind_end]

        self.img_raw_transform = transforms.ToTensor()
        self.img_transform = transforms.Compose([
            transforms.Resize(CVT_IMG_SIZE),
            transforms.ToTensor(),
            transforms.Normalize(mean=DATASET_MEAN, std=DATASET_STD),
        ])

    def __len__(self):

        '''
        Get the length of the dataset.
        '''

        return len(self.annotation)

    def __getitem__(self, idx):

        '''
        Get an item of the dataset.
        '''
        
        # Get the parameters.
        img_name = self.dataset_names[idx]
        img_raw = Image.open(os.path.join(self.dataset_path, img_name))
        image = self.img_transform(img_raw)
        img_raw = self.img_raw_transform(img_raw)
        
        # Transform the data.
        keypoints = project_points(self.annotation[idx], self.matrix[idx])
        keypoints = keypoints / ORG_IMG_SIZE
        heatmaps = array_to_heatmaps(keypoints)
        keypoints = torch.from_numpy(keypoints)
        heatmaps = torch.from_numpy(np.float32(heatmaps))

        return {
            "image": image,
            "keypoints": keypoints,
            "heatmaps": heatmaps,
            "image_name": img_name,
            "image_raw": img_raw,
        }

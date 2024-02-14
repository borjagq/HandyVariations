<a name="readme-top"></a>
<!-- PROJECT LOGO -->
<br />
<div align="center">

  <img src="https://github.com/borjagq/HandyVariations/blob/main/HandyVariations/resources/icon.png?raw=true" alt="Logo" width="240">

  <br />

  <h3 align="center">HandyVariations</h3>

  <p align="center">
    This repository contains source files developed for my MSc dissertation titled, "Procedural Generation of Datasets for Training Hand Pose Estimation Systems", at the University of Dublin, Trinity College.
    <br />
    <br />
  </p>
  <span align="center"> 
  
  [![Made with OpenGL](https://img.shields.io/badge/Built%20with%20OpenGL-%23FFFFFF.svg?style=flat&logo=opengl)](https://www.opengl.org)
  [![Made with Xcode](https://img.shields.io/badge/Built%20with%20Xcode-007ACC?style=for-the-badge&logo=Xcode&logoColor=white&style=flat")]([https://](https://developer.apple.com/xcode/))
  
  </span>
</div>


<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#video-demo">Demo Video</a></li>
        <li>
          <a href="#built-with">Built With</a>
          <ul>
            <li><a href="#synthetic-dataset-generation">Synthetic Dataset Generation</a></li>
            <li><a href="#hand-pose-estimation-dataset-validation">Hand Pose Estimation (Dataset Validation)</a></li>
          </ul>
        </li>
      </ul>
    </li>
    <li>
      <a href="#usage">Usage</a>
        <ul>
          <li><a href="#synthetic-dataset-generation">Synthetic Dataset Generation</a></li>
          <li><a href="#hand-pose-estimation-dataset-validation">Hand Pose Estimation (Dataset Validation)</a></li>
        </ul>
    </li>
    <li><a href="#acknowledgement">Acknowledgement</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#licence">Licence</a></li>
    <li><a href="#references">References</a></li>
  </ol>
</details>


<!-- ABOUT THE PROJECT -->
## About The Project

<div align="center">
  <figure>
    <img src="https://github.com/borjagq/HandyVariations/blob/main/HandyVariations/resources/example.png?raw=true" alt="Generated image example" width="256">
    <figcaption>Generated image example</figcaption>
</figure>
</div>

The rise of computer vision systems has reshaped many industries, driven by the powerful capabilities of deep neural networks. However, as the complexity of these systems grows, so does the demand for larger datasets. However, the manual annotation of large-scale datasets comprising the diversity required by these systems is labour-intensive and time-consuming. This dissertation delves into the exploration of procedural generation techniques for hand pose datasets to overcome this challenge while also investigating the impact of controlled variations in detection quality and reliability, encompassing joint angles, wrist orientations, texture, lighting, and background variations, aiming to make it capable of handling diverse real-world settings. To assess the efficacy of the generated datasets, a state-of-the-art computer vision system is trained to detect key points in hand images using both the procedurally generated dataset and traditionally annotated datasets. Comparative analyses evaluate the trained system's performance on real-world data, comprehending the influence of procedural variations on its accuracy, robustness, and generalisation capabilities. In conclusion, this dissertation contributes to hand pose estimation by integrating innovative approaches for procedurally generating datasets. The findings underscore the importance of automated variations in dataset generation and offer insights into their impact on the quality of trained computer fabrication systems.

Abstract from <a href="#references">[1]</a>

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- DEMO VIDEO -->
### Demo Video

[![HandyVariations - Demo Video](https://img.youtube.com/vi/eR6k7Kn73kU/0.jpg)](https://www.youtube.com/watch?v=eR6k7Kn73kU)

<p align="right">(<a href="#readme-top">back to top</a>)</p>


### Built With

#### Synthetic Dataset Generation

- Programming Language:
  - C++ (C++20 dialect)
- Graphics Library:
  - OpenGL 4.1
- Compilation Environment:
  - Xcode Version 14.3.1 (14E300c) on macOS Ventura 13.4.1, compiled for an Apple M1 Pro CPU/GPU
- Additional Libraries:
  - Glew (version 2.2.0)
  - GLFW (version 3.3.8)


#### Hand Pose Estimation (Dataset Validation)

- Programming Language:
  - Python (version 3.9.6)
- Machine Learning Library:
  - PyTorch (version 2.0.1)

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- Before use -->
## Usage

1. Clone this repository.

```
git clone https://github.com/borjagq/HandyVariations/
```

### Synthetic Dataset Generation

2. The 3D model used in this software is not included in the repository due to commercial licensing. You can download the model from [this link](https://www.cgtrader.com/3d-models/character/man/rigged-hand-model-photorealistic-with-nails). Place the downloaded files in the models folder with the following names:

```
hand.fbx
textures/hand_specular.jpg
textures/hand_base_color.jpg
textures/hand_normals.jpg
```

3. Download a 'random' image dataset, such as [unlabeled COCO](https://cocodataset.org/#download) images. Place the images in a dedicated directory with no other files.

4. Run the script `rename_rdm_bg.sh` located in the `Scripts` directory, followed by the directory where you stored the random images. **Ensure no other files are present, as they will be renamed.**

```
zsh rename_rdm_bg.sh /Path/To/Random/Images/
```

5. Open the Xcode project `HandyVariations.xcodeproj`.

6. Build it and run it.


### Hand Pose Estimation (Dataset Validation)

7. Download all the required Python modules.

```
pip3 install numpy matplotlib tqdm torch torchvision Pillow opencv-python
```

8. Open the Jupyter Notebooks (.ipynb) and run them for dataset validation.

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- ACKNOWLEDGEMENTS -->
## Acknowledgement

The software available in this repository has been specifically developed for the experiments conducted in the Dissertation <a href="#references">[1]</a> presented by the author to the University of Dublin, Trinity College in partial fulfilment of the requirements for the degree of MSc in Computer Science (Augmented and Virtual Reality). It serves as a dedicated tool tailored to address the requirements and methodologies outlined in the research.

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- CONTACT -->
## Contact

**Borja García Quiroga, MSc**

- Github: <a href="https://github.com/borjagq/">@borjagq</a>
- Website: <a href="https://borjagq.com/">borjagq.com</a>

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- LICENCE -->
## Licence

HandyVariations is licensed under the GNU GPLv3, see [LICENSE](https://github.com/borjagq/HandyVariations/blob/main/LICENSE) for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- REFERENCES -->
## References

* <a href="https://borjagq.com/2023/09/10/msc-dissertation/">[1] B. García Quiroga, “Procedural Generation of Datasets for Training Hand Pose Estimation Systems,” dissertation, 2023 </a>

<p align="right">(<a href="#readme-top">back to top</a>)</p>

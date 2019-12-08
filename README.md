# OCR
Optical character recognition program based on deep learning.

## How to use

1. Clone this repository with `git clone git@github.com:GabRayz/OCR.git`.
2. Compile the project by running the command `make` in the project folder.
3. Execute the program with `./ocr`.

### Graphical User Interface

The character recognition program can be used through a GUI. Use `./ocr` to launch it. Drag and drop an image in the GUI to process it. The extracted text is shown on the left side. The result can then be saved into a txt file.

### Command Line Interface

The CLI offers more functionalities. Type `./ocr help` to list available commands. Type `./ocr {command}` to see the usage of the command.
- `./ocr write_dataset`: Creates a new learning dataset from a directory containing images to be segmented. Each image must contain the following string : `0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ'()-_.,?!:;`.
- `./ocr learn`: Trains a new neural network from a given dataset. The so created network can be saved.
- `./ocr read`: Reads an image using the given neural network. The extracted text is displayed and saved into a txt file.

## Dependencies

- `SDL2` : `SDL2` is used to make the `Graphical User Interface`.
- `Image Magick 7` : Import, resize, rotate images.
- `Hunspell 7` : `Hunspell` is a spell checker and morphological analyzer designed for languages with rich morphology and complex word compounding and character encoding.

## Features

- Import images
- Preprocessing : 
  - Greyscale
  - Noise Reduction
  - Contrast Enhancement
  - Binarization (`Otsu's Method`)
  - Rotation Detection (`Hough's Transformation`)
  - Rotation Correction
- Segmentation :
  - Paragraphs & Lines Detection (`X-Y Cut`)
  - Characters Detection (Connected Component Labeling `CCL`)
- Recognition :
  - Neural Network Loading
  - Neural Network Recognition
- Postprocessing :
  - Uppercase removal
  - SpellCheck (`Hunspell`)
- Display (Graphical User Interface)

## Collaborators

- Gabriel Rayzal : github.com/GabRayz
- Tony Heng : github.com/TonyHg
- Nathan Cabasso : github.com/Vardiak

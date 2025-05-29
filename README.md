# Open NMR

> Warning
>
> **This project is in active development and is a hobby project. It is not a finished product and is subject to change. Use at your own risk.**




## Overview

Open NMR will be a simple and free application for processing and analysis of NMR spectra

![usage example](usage_example.png)

## Features

At this moment program can open 1D NMR spectra from Bruker, Agilent and Varian Spectrometers. Current features:

- phase correction (only manual for now)
- integration and seting integral values (by double clicking on label)
- zero filling and truncation


## Installation

Download current version from https://github.com/jp-nowak/open-nmr-cpp/releases and unpack in chosen folder.
Application is started by running open-nmr.exe


## Dependencies

- Qt Framework 6.9
- kissfft https://github.com/mborgerding/kissfft
(should be cloned into /src/app/3rd_party for compilation

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.



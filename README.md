# NEVADA (NetCDF Explorer and Visualizer for Atmospheric Data Analysis)

## Presentation
NEVADA (NetCDF Explorer and Visualizer for Atmospheric Data Analysis) is a small software that allows to open and display bidimensionnal variables from NetCDF files.

This soft was designed to handle files around 300 Mo.

![](nevada_presentation.png)

## Licence
This project is all under GNU General Public Licence v3.0.
Please read LICENCE for more informations.

## Usage

### Dependencies
To build and use NEVADA, you will need the following:

 - netcdf >= 4.9.0
 - gtk3
 - SDL2

### Installation
To install NEVADA, just follow these steps:
```
$ git clone https://github.com/Shadow15510/NEVADA.git 
$ cd NEVADA
$ make
$ make install
```
If you want to delete the objects files from the compilation you can `make clean`.

### Update
To update NEVADA:
```
$ cd NEVADA
$ make clear
$ git pull
$ make
$ make install
```
Note that `make clear` isn't necessary (actually it allows to supress NEVADA from your system).

### Uninstall
To remove NEVADA from your system:
```
$ cd NEVADA
$ make clear
$ cd ../
$ rm -rf NEVADA
```

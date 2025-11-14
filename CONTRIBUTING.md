# Contributing to project

## How to Contribute
1. Report bugs
2. Suggest features  
3. Submit pull requests
4. Improve documentation

## Development Setup

### Clone 
```bash 
# clone the dev branch
git clone https://gitlab.basealt.space/proskurinov/surguch/-/tree/dev?ref_type=heads
cd surguch
```
### Install dependencies
    qt6-declarative-devel  
    qt6-svg-devel                   
    qt6-tools               
    libmupdf-devel         
    libaltcsp-devel        
    libcsppdf-devel        
    gcc-c++                
    cmake                  
    ninja-build  

### Build
```bash 
cmake -S . -B build
cmake --build build
```

## Code Style
[CODESTYLE.md](CODESTYLE.md)

## Pull Request Guidelines
* Create a branch
* Make your changes 
* Submit the **Merge Request**.

## Issue Reporting
* Create Gitlab Issue [here](https://gitlab.basealt.space/proskurinov/surguch/-/issues) 
* Or you can report via the [ALT Linux Bugzilla](https://bugzilla.altlinux.org/)
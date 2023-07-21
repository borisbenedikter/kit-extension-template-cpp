# Building a Kit App

## Test That Kit Is Installed Correctly on Your Machine

After installing Kit in Omniverse, you can test that your machine is ready to run Kit applications by running the following command in a terminal:

If any of this step fails, please see the [Troubleshooting](#troubleshooting) section below.

1. Clone the [GitHub repo](https://github.com/NVIDIA-Omniverse/kit-extension-template-cpp) to your local machine.
2. Open a command prompt and navigate to the root of your cloned repo.
3. Run `build.bat` to bootstrap your dev environment and build the example extensions.
4. Run `_build\{platform}\release\omni.app.example.extension_browser.bat` to open an example kit application.
    - Run `omni.app.example.viewport.bat` instead if you want the renderer and main viewport to be enabled.
    - Run `omni.app.kit.dev.bat` instead if you want the full kit developer experience to be enabled.
5. From the menu, select `Window->Extensions` to open the extension browser window.
6. Enter `omni.example.cpp` in the search bar at the top of the extension browser window to view the example extensions included with this repo.

### Troubleshooting

#### `build.bat` Fails With `ERROR: Docker required`

In this case, you need to install Docker. Please see the [Docker installation instructions](https://docs.docker.com/get-docker/) for your platform.

Note: I used [this guide by NVIDIA](https://docs.nvidia.com/ai-enterprise/deployment-guide-vmware/0.1.0/docker.html), but it contained a few errors.
For instance,you will need to manually change the file `/etc/apt/sources.list.d/archive_uri-https_download_docker_com_linux_ubuntu-jammy.list` and be sure to replace its contents with the following single line:

```
deb [arch=amd64] https://download.docker.com/linux/ubuntu jammy stable
```

#### `extension_browser` App Returns Warning Message on Disabling Iommu

To disable IOMMU, you need to reboot your machine and enter the BIOS settings.
You need to press a key (usually `F2` or `DEL`) during the boot process to enter the BIOS settings.
Tip: on Linux you can specify the next boot to go directly to the BIOS settings by running `sudo systemctl reboot --firmware-setup`.

Once in the BIOS settings, you need to find the IOMMU setting and disable it.
In MSI BIOS settings, the IOMMU is under the OC settings, and is visible only after enabling the Expert mode of the CPU settings.

## Building an Extension

To create a new extension,

1. Copy one of the existing extension examples to a new folder within the `source/extensions` folder.
    - The name of the new folder will be the name of your new extension.
    - The **omni** prefix is reserved for NVIDIA applications and extensions.
    This is why I use `boris` as the prefix for my extensions.
2. Update the fields in your new extension's `config/extension.toml` file as necessary.
Essentially, you need to change `omni` or `boris` to your own prefix.
3. Update your new extension's `premake5.lua` file as necessary.
Essentially, you need to change `omni` or `boris` to your own prefix.
4. Update your new extension's C++ code in the `plugins` folder as necessary.
5. Update your new extension's Python code in the `python` folder as necessary.
6. Update your new extension's Python bindings in the `bindings` folder as necessary.
7. Update your new extension's documentation in the `docs` folder as necessary.
8. Update the name of the folders `omni` or `boris` with your own prefix.
9. Run `build.sh` to build your new extension. This may take a couple of minutes (be patient).
10. Run `_build\{platform}\release\omni.app.example.extension_browser.sh` to open an example kit application.
11. Enter the name of your new extension in the search bar at the top of the extension browser window to view it. Once you find it, you can enable/disable it. The output will be printed in the terminal where you ran the `omni.app.example.extension_browser.sh` command.

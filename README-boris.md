# Setting up a Kit app

## Test that Kit is installed correctly on your machine

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

## Troubleshooting

### `build.bat` fails with `ERROR: Docker required`

In this case, you need to install Docker. Please see the [Docker installation instructions](https://docs.docker.com/get-docker/) for your platform.

Note: I used [this guide by NVIDIA](https://docs.nvidia.com/ai-enterprise/deployment-guide-vmware/0.1.0/docker.html), but it contained a few errors.
For instance,you will need to manually change the file `/etc/apt/sources.list.d/archive_uri-https_download_docker_com_linux_ubuntu-jammy.list` and be sure to replace its contents with the following single line:

```
deb [arch=amd64] https://download.docker.com/linux/ubuntu jammy stable
```

### `extension_browser` app returns warning message on disabling IOMMU

To disable IOMMU, you need to reboot your machine and enter the BIOS settings.
You need to press a key (usually `F2` or `DEL`) during the boot process to enter the BIOS settings.
Tip: on Linux you can specify the next boot to go directly to the BIOS settings by running `sudo systemctl reboot --firmware-setup`.

Once in the BIOS settings, you need to find the IOMMU setting and disable it.
In MSI BIOS settings, the IOMMU is under the OC settings, and is visible only after enabling the Expert mode of the CPU settings.

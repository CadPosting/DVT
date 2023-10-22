import tkinter as tk
import subprocess

def configure_build():
    # Run the cmake configuration command
    subprocess.Popen("cmake .. -G \"MinGW Makefiles\" -DPython3_EXECUTABLE=\"C:/Python310-32/python3.exe\" -DPython3_INCLUDE_DIRS=\"C:/Python310-32/include\" -DPython3_NumPy_INCLUDE_DIRS=\"C:/Python310-32/lib/site-packages/numpy/core/include\" -DCMAKE_C_COMPILER=\"C:/MinGW/bin/gcc.exe\" -DCMAKE_CXX_COMPILER=\"C:/MinGW/bin/g++.exe\" -DLIBXL_INCLUDE_DIR=\"C:/libxl-win-4.1.2/libxl-4.1.2/include_cpp\" -DLIBXL_LIBRARY_DIR=\"C:/libxl-win-4.1.2/libxl-4.1.2/lib\"", shell=True, cwd="C:\\DVT\\build")

def execute_build():
    # Run the cmake build command
    subprocess.Popen("cmake --build .", shell=True, cwd="C:\\DVT\\build")

def output():
    # Run the output executable
    subprocess.Popen(".\\output.exe", shell=True, cwd="C:\\DVT\\build")

# Create the main window
root = tk.Tk()

configure_button = tk.Button(root, text="Configure Build", command=configure_build)
configure_button.pack()

execute_button = tk.Button(root, text="Execute Build", command=execute_build)
execute_button.pack()

run_button = tk.Button(root, text="Run Project", command=output)
run_button.pack()

# Start the GUI event loop
root.mainloop()
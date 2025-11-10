import os
from conan import ConanFile
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout

class WindowsInstallerProbe(ConanFile):
	settings = 'os', 'compiler', 'build_type', 'arch'
	
	def requirements(self):
		if os.getenv('COMPUTERNAME', '') == 'FR-L-F0DW424':
			self.requires('qt-binaries/6.7.3')
			self.is_binaries = 'qt-binaries'
		else:
			self.requires('qt/6.7.3')
			self.qt_package = 'qt'
	
	def generate(self):
		deps = CMakeDeps(self)
		deps.generate()
		tc = CMakeToolchain(self)
		tc.variables["QT_PACKAGE_NAME"] = self.qt_package
		tc.generate()
 
	def layout(self):
		cmake_layout(self)
	
	def build(self):
		cmake = CMake(self)
		cmake.configure()
		cmake.build()

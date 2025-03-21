from conan import ConanFile

class SpectrumServiceHardwareConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = [
        "gtest/1.14.0",
        "spdlog/1.12.0"
    ]
    generators = "CMakeDeps", "CMakeToolchain"

    def configure(self):
        self.options["gtest"].shared = True


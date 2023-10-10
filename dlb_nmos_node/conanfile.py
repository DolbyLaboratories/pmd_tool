from conans import ConanFile, CMake, tools


class DlbNmosNodeConan(ConanFile):
    name = "dlb_nmos_node"
    version = "2.0"

    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake_find_package", "cmake_paths", "cmake_find_package_multi", "cmake", "make"

    requires = "boost/1.75.0", "openssl/1.1.1i", "cpprestsdk/2.10.17", "websocketpp/0.8.2", "zlib/1.2.11"

    scm = {
        "type": "git",
        "url": "https://gitlab-sfo.dolby.net/professional-metadata/dlb_nmos_node.git",
        "revision": "auto"
    }

    def configure(self):
        self.options["boost"].shared = False

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def build(self):
        cmake = CMake(self)
        cmake.definitions["USE_CONAN"]=False
        cmake.definitions["CMAKE_BUILD_TYPE"]="Release"
        cmake.configure(source_folder="./Development")
        cmake.build()

    def package(self):
        libs = ["libdlb_nmos_node_bundled" ]

        lib_format = "a"

        if self.settings.os == "Windows":
            lib_format = "lib"

        for lib in libs:
            self.copy("{}.{}".format(lib, lib_format), dst="lib", keep_path=False)

        self.copy("dlb_nmos_node_api.h", src="Development/dlb-nmos", dst="include")

    def package_info(self):
        self.cpp_info.libs = ["dlb_nmos_node_bundled"]
        
        if self.settings.os != "Windows":
            self.cpp_info.system_libs = ["dns_sd"]

    def layout(self):
        self.folders.build = "Development/build"

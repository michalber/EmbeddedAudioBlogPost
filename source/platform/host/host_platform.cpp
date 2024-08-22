#include "host_platform.hpp"

#include <iostream>
#include <memory>

static std::unique_ptr<CPlatform> gPlatform {};

CPlatform_Host::CPlatform_Host() {}

CPlatform_Host::~CPlatform_Host()
{
}

void CPlatform::Assert(const char *rpMsg, const char *rpFileName, int rdLine) {}

void CPlatform::LaunchPlatform() {
    gPlatform = std::make_unique<CPlatform_Host>();
}

void CPlatform_Host::InitDrivers() {
}

void CPlatform_Host::InitPlatform() {
    std::cout << "Hello world!" << std::endl;
}
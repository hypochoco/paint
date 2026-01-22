//
//  utils.m
//  paint
//
//  Created by Daniel Cho on 12/28/25.
//

#import <Foundation/Foundation.h>

#include <string>

#include "paint/utils.h"

bool isLowPowerModeEnabled() {
    if (@available(macOS 12.0, *)) {
        return [[NSProcessInfo processInfo] isLowPowerModeEnabled];
    }
    return false;
}

std::string resolveBundlePath(const std::string& resourceName) {
    NSString* name = [NSString stringWithUTF8String:resourceName.c_str()];
    NSString* path = [[NSBundle mainBundle] pathForResource:name ofType:nil];

    if (!path) {
        throw std::runtime_error("Resource not found in bundle: " + resourceName);
    }

    return std::string([path UTF8String]);
}

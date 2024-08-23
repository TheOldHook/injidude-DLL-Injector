// SDK.h
#pragma once

// Example SDK function prototype
typedef void(*SomeGameFunction)();

// You would typically find the address of this function in the game and then call it like this:
// SomeGameFunction someFunction = (SomeGameFunction)(baseAddress + 0xABCDEF);
// someFunction();
#pragma once

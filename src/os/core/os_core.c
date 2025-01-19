// Copyright Frost Gorilla, Inc. All Rights Reserved.

// Handle functions
internal OS_Handle os_handle_zero(void) { return (OS_Handle){0}; }

internal b32 os_handle_match(OS_Handle a, OS_Handle b) { return a.handle[0] == b.handle[0]; }

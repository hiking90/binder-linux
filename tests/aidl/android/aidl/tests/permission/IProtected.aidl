package android.aidl.tests.permission;

interface IProtected {
    @EnforcePermission("READ_PHONE_STATE") void PermissionProtected();

    @EnforcePermission(allOf={"INTERNET", "VIBRATE"}) void MultiplePermissionsAll();

    @EnforcePermission(anyOf={"INTERNET", "VIBRATE"}) void MultiplePermissionsAny();

    @EnforcePermission("android.net.NetworkStack.PERMISSION_MAINLINE_NETWORK_STACK")
    void NonManifestPermission();

    // Used by the integration tests to dynamically set permissions that are considered granted.
    @RequiresNoPermission void SetGranted(in List<String> permissions);
}

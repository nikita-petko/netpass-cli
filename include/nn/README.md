# Special CTR-SDK headers

This explains why some of these headers are here.

## nn::am

These headers are here in order for the functions:

- `nn::am::InitializeForSystemMenu()`
- `nn::am::FinalizeForSystemMenu()`
- `nn::am::GetDeviceId(bit32* pOut)`

These are used to retrieve the console's device ID to set it to the 3ds-nid header when making requests to NetPass.<br />
The initialize and finalize functions are to complement the get device id function.

## nn::applet

These headers are here to access the `nn::applet::CTR::detail::CloseApplicationCore` method.<br />
The default method that is provided (`nn::applet::CloseApplication`) will throw an assertion error if it used when `nn::applet::IsExpectedToCloseApplication` is false, `CloseApplication` is essentially just a NN_TASSERT_() call and then a `CloseApplicationCore` call immediately after it.

The entire point of this method is to facilitate closing the application with the <kbd>start</kbd> button.

## nn::http

These headers are here to allow HTTP calls, as of now this is only a temporary solution until [libcurl](https://curl.haxx.se/libcurl/) is ported to the CTR-SDK via `nn::socket`.

This library is very limited in the protocol, it only supports up to HTTP/1.1 and TLSv1.1 (with is obsolete, and the great majority of servers reject this protocol version now), because of that I have to proxy the NetPass servers with an insecure proxy site.

## nn::nwm

These headers are here in order for the functions and classes:

- `nn::nwm::GetMacAddress(Mac& mac)`
- `nn::nwm::Mac`

These are used to retrieve the console's MAC address to set it to the 3ds-mac header when making requests to NetPass.<br />

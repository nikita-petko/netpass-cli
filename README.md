# NetPass: a new way to experience StreetPass!

This is an extension of the original [NetPass](https://gitlab.com/3ds-netpass/netpass) that approaches it with more of a
CLI type approach, this app has very simple UI but is much faster than the original app, this app is also built with the native
SDK.

GBAtemp thread: https://gbatemp.net/threads/a-new-way-to-experience-streetpass.653810/  
Discord guild: https://discord.gg/netpass

Introducing NetPass:

In the current state of the world, the 3DS is, sadly, on decline. That makes getting StreetPasses harder and harder, due to fewer and fewer people taking their 3DS with them on a walk. This is where NetPass comes in!  
Unlike StreetPass, NetPass works over the internet. Upon opening NetPass, you can pick various locations to go to, i.e. the train station, or a town plaza. Upon entering a location, you get passes of others who are in the same location! And, while you are at the location, others who enter it can get passes with you. But beware! You can only switch locations once every 10 hours!

## Notice for HTTP

This application currently depends on an insecure proxy of [the API](https://api.netpass.cafe) in order to comply with a limitation of nn::http.
This will be changed in a future release when [CTR libcurl](https://github.com/nikita-petko/ctr-libcurl) is completed.

## Notice for box updates

Currently when this updates the box, it does not set the notification light properly for the title, in the future this will be fixed when I drop the dependency on nn::cec::MessageBox and just do direct reads and writes from the files.

## Translations
If you want to contribute to translations, we are doing that on [our Weblate](https://weblate.sorunome.de/projects/netpass)!

## Credits
 - [NetPass](https://gitlab.com/3ds-netpass/netpass) by [Sorunome](https://gitlab.com/Sorunome), the original implementation.

## License

This project is licensed under the Apache-2.0 License:

```
   Copyright 2024 MFDLABS

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
```

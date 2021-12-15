This sourcecode is a plugin base with SDK to code your own server plugins.

Please read the iface.cpp carefully!

You have to export the following functions:
* CCS_PluginInit: Called when plugin is in loading progress. There you get data objects and object pointers which are needed for your plugin.
* CCS_PluginInfos: Called when the engine wants to get your plugin informations.
* CCS_PluginFree: Called when plugin is unloaded.

In general you have to export functions, which are called from the engine.
They are intented to retrieve data objects or object pointers,
to tell the engine informations about the plugin (which can be seen by admins)
and also to tell the plugin when it gets unloaded. If you want your plugin
to be informed for specific events, then you have to tell the engine for
which event you want your plugin to be informed. There are two different
types of events, the first is called before the engine does its job and the
second is called after the engine has finished its job. Of course you have
a bit of control to the engine by using specific return values. Well, this
description is abstract, but just have a view at the provided sourcecode,
you will then see that writing plugins is very simple but useful.
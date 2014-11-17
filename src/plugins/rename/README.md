- infos = Information about keytometa plugin is in keys below
- infos/author = Felix Berlakovich <elektra@berlakovich.net>
- infos/licence = BSD
- infos/needs =
- infos/provides = filter
- infos/placements = presetstorage postgetstorage
- infos/description = renaming of keys

## INTRODUCTION ##


This plugin can be used to perform rename operations on keys passing by. This might be useful if a backend does not provide keys
in the required format. If keys are renamed, their original name is stored in the `origname` MetaKey.


## CUT ##

### OPERATION ###

The cut operation can be used to strip parts of a keys name. The cut operation is able to cut anything starting after the path
of the parent key. However, the resulting name must not be equal to the parent key. For example consider a KeySet with the 
parent key `user/config`. If the KeySet contained a key with the name `user/config/with/long/path/key1`, the cut operation
would be able to strip the following key name parts:
* with
* with/long
* with/long/path

However, it would refuse to strip the following
* with/long/path/key1

The reason is, that stripping the latter would cause a parent key duplicate.

### CONFIGURATION ####

The cut operation takes as its only configuration parameter the key name part to strip. This configuration can be supplied in two
different ways. First, the global configuration key `cut` can be used. Second, keys to be stripped can be tagged with the MetaKey `rename/cut`.
If both options are given, the MetaKey takes precedence. For example, consider the following setup:

				`config/cut` = will/be
				parent key = user/config
								
				user/config/will/be/stripped/key1			<- meta rename/cut = will/be/stripped
				user/config/will/be/stripped/key2			<- meta rename/cut = will/be/stripped
				user/config/will/be/stripped/key3
				user/config/will/not/be/stripped/key4

The result of the cut operation would be the following KeySet:

				user/config/key1
				user/config/key2
				user/config/stripped/key3
				user/config/will/not/be/stripped/key4
				
The cut operation is agnostic to a single trailing slash in the configuration. This means that it makes no difference whether `cut = will/be/stripped`
or `cut = will/be/stripped/`. However, the cut operation refuses cut paths with leading slash. This is to clarify that key name parts can only be stripped
after the parent key path.

If an invalid configuration is given or the cut operation would cause a parent key duplicate, the affected keys are simply skipped and not renamed. 


## PLANNED OPERATIONS ##


Additional rename operations are planned for future versions of the rename plugin:
* trim: remove spaces in the name (that are not part of parentKey!)
* case: upper/lowercase the name (that are not part of parentKey!)

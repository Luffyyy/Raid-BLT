# Raid BLT
An open source Lua hook for Raid that is a fork of the payday 2 BLT hook, designed and created for ease of use for both players and modders.  
This is the developer repository.
The Lua component of the BLT which controls mod loading can be found in it's own repository, [Raid-BLT-Lua](https://github.com/ModWorkshop/Raid-BLT-Lua).

## Download
Visit [ModWorkshop](https://modworkshop.net/mydownloads.php?action=view_down&did=21065) to get the latest stable download. 

## Documentation
Documentation for the BLT can be found on the [GitHub Wiki](https://github.com/ModWorkShop/Raid-BLT/wiki) for the project.

## Dependencies
Raid BLT requires the following dependencies, which are all statically linked.
* OpenSSL
* cURL
* zlib
* MinHook

### OpenSSL
OpenSSL should be compiled as static libraries and the libraries placed in the lib directory, and the headers in the incl directory

### cURL
cURL should be compiled as static, with the WITH_SSL parameter set to 'static' and the previously compiled OpenSSL libraries in the dependencies directory.

### zLib
zLib should be compiled as static.
I had to add SAFESEH handling to the MASM objects in order for this to be compatible with Payday2-BLT

### MinHook
A compiled version of MinHook is included

## Contributors
- Payday 2 BLT Team
	* [James Wilkinson](http://jameswilko.com/) ([Twitter](http://twitter.com/_JamesWilko))
	* [SirWaddlesworth](http://genj.io/)
	* [Will Donohoe](https://will.io/)

- Contributors, Translators, Testers and more
	* saltisgood
	* Kail
	* Dougley
	* awcjack
	* BangL
	* Cryfact
	* shinrax2
	* chromKa
	* xDarkWolf
	* Luffyyy
	* NHellFire
	* TdlQ
	* Mrucux7
	* Simon
	* goontest
	* aayanl
	* cjur3
	* Kilandor
	* Joel Juvél
	* PlayYou
	* Snh20
	* and others who haven't been added yet

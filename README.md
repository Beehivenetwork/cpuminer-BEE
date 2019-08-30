cpuminer-BEE is a fork of cpuminer-opt & cpuminer-multi by TPruvot with optimizations
imported from other miners developped by lucas Jones, djm34, Wolf0, pooler,
Jeff garzik, ig0tik3d, elmad, palmd, barrystyle, Jay D Dee and Optiminer, with additional
optimizations by BeeHIVE.

See file RELEASE_NOTES for change log and INSTALL_LINUX or INSTALL_WINDOWS
for compile instructions.

Requirements
------------

1. A x86_64 architecture CPU with a minimum of SSE2 support. This includes
Intel Core2 and newer and AMD equivalents. In order to take advantage of AES_NI
optimizations a CPU with AES_NI is required. This includes Intel Westbridge
and newer and AMD equivalents. Further optimizations are available on some
algoritms for CPUs with AVX and AVX2, Sandybridge and Haswell respectively.

Older CPUs are supported by cpuminer-multi by TPruvot but at reduced
performance.

ARM CPUs are not supported.

2. 64 bit Linux OS. Ubuntu and Fedora based distributions, including Mint and
Centos, are known to work and have all dependencies in their repositories.
Others may work but may require more effort. Older versions such as Centos 6
don't work due to missing features. 
64 bit Windows OS is supported with mingw_w64 and msys or pre-built binaries.

MacOS, OSx and Android are not supported.

3. Stratum pool. Some algos may work wallet mining using getwork or GBT. YMMV.


[![#](https://i.imgur.com/A1dwLWw.jpg)](https://twitter.com/BeeHiveNetworks)



YESpower Algorithms
--------------------

                          yespower      Cryply
                          yespowerr16   Yenten (YTN)
                          yespowerurx   Yespower (URX)
                          yespowerlitb  Yespower (LITB)
                          cpupower      CPUchain (CPU)


![#](https://i.imgur.com/wKq9ym4.png)

project LightBIT...
-------------------

[![#](https://i.imgur.com/3oH3PSj.png)](https://lightbit.xyz)

Pools...
--------

[![#](https://i.imgur.com/71pg2cu.jpg)](https://litb.skypool.co)[![#](https://i.imgur.com/hxgnbOu.png)](http://hashpex.com/)
		
Tricks...
---------

checked & set "hugepages"
cat /proc/meminfo | grep Huge 
sudo sysctl -w vm.nr_hugepages=2048

play & set --affinity parameter
https://phempshall.github.io/cpuminer-affinity-setter/


Donations
---------

cpuminer-BEE has no fees of any kind but donations are accepted.

 BTC: 3Qxyq3QDfaU2t8h4rDTTPTHgTdQc5bCJXd
 LTC: MSYzi5zSCwEJsPy7FVDQiUsZkK9iJW3Mxz

Happy mining!

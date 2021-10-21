
                                                            AnFu Virtual PC/SC Reader Driver
                                     --------------------------------------------------------------------
                                                                          Apr. 12 2008



[ Introduction ]
=========================

        This driver package is designed to demostrate my 'SmartCard demo' without any real smartcard and PC/SC reader.
For more information about the 'SmartCard demo', please refer to:  http://www.codeproject.com/KB/system/Smartcard.aspx

        This virtual driver has a funny function: it can monitor your USB device as the smart card. Well, when you plug-in a 
USB pen, the smart card inserted; otherwise, your smart card ejected. ;-), So, if you want to test it, you should prepare a 
USB pen.

        Supported OS: Win2000/XP/2003/Vista. XP/2003 tested and passed.

[ Install driver ]
=========================

        Using the 'sc_inst.exe' to install this virtual driver - you should put 'VCardRdr.inf' and 'VCardRdr.sys' at the same path
of 'sc_inst.exe';

        If you get a dialog to prompt: 'this driver is not signed', just select 'Yes'.


[ Uninstall driver ]
=========================

        Before uninstall this driver, you should close any connection to the 'AF Virtual Reader'.

        Launch the device manager, naviage to the 'Smartcard reader' group, and select the ''Virtual PnP SmartCard Device" 
to uninstall.


[ Simple APDU set ]
=========================

        As a demostrating, the virtual dirver has a minal set COS commands:

        * Get Challenge
           0084 0000 08              - get 8 bytes challenge
           0084 0000 04              - get 4 bytes challenge

        * Select a file
           00A4 0000 02 3F00     - select the 3F00 DF. ( you can select any object, and it will be always O.K. )

        * Read data
           00B0 0000 04              - read 4 bytes from COS. ( in fact: c:\smcard.img )

        * Write data
           00D0 0000 04 11223344             - write 4 bytes to COS. ( in fact: c:\smcard.img )

        * NOTE: internal buffer has 300 bytes limited, so each APDU should be less than 300 bytes.

        So, we can using the following script in the 'SmartCard Demo' - Advanced edition:

-------------------------------- script begin
0084 0000 08
# exp: 
00a4 0000 02 3f00
# exp: 
00D0 0000 02 1122
# exp: 
00B0 0000 02
# exp: 
-------------------------------- script end

        In this script, we get a challenge of 8 bytes, select the '3F00' object, write 2 bytes to COS, and read the 2 bytes 
from COS. ^_^


[ Special thanks ]
===========================

        This virtual driver is my dream. I want to apply it for a long time after i get to know the limitation of Dmitry's
virtual driver. And recentlly, i get time to do so.

        So, thanks Dmitry, for your first virtual smartcard driver. And the virtual APDU of my virtual driver function is 
based on Dmitry's.


[ Using Term ]
-----------------------------------------
        * This driver is for testing purpose only, it has nothing to do with any damage of your resources!
        * Any business purpose of using this driver is not recommended.

----------------------------------------------------------------------------------------------------------------------
Y.Volta (y.volta@gmail.com)
22:41 2008-4-12


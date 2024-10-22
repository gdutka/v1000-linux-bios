
TlsDxe with OPENSSL integrate guide.

  1. To integrate TlsDxe with OPENSSL library source code package.
    Download OPENSSL libaray source package from URL https://github.com/openssl/openssl/tree/OpenSSL_1_1_1w
    Decompress package to the path InsydeNetworkPkg/Library/OpensslLib/openssl-1.1.1w

  2. Enable FeaturePCD in Project.dsc

    gInsydeTokenSpaceGuid.PcdH2ONetworkHttpSupported|TRUE
    gInsydeTokenSpaceGuid.PcdH2ONetworkTlsSupported|TRUE

  3. Build project BIOS, make sure the TlsDxe has be generated.

  OPENSSL configuration:
    OpensslLib/include/openssl/opensslconf.h
    OpensslLib/include/crypto/dso_conf.h


Test TlsDxe with ImportCA.efi and RestTest.efi

  Get EFI files from InsydeNetworkPkg/Application, the binary files is exist in ImportCA and RestTest folder, please find it as your arch.

  If you want to build it as DEBUG build, please run build command

    build -p InsydeNetworkPkg/InsydeNetworkPkg.dsc -a $(YOUR_ARCH) -b DEBUG

  The EFI files will be generated in $(WORKSPACE)/Build/InsydeNetworkPkg/DEBUG_$(TOOL_CHAIN)/$(ARCH)


Create Certificate file steps.

  TlsDxe will process the TLS request from driver HttpDxe. It will not process by itself.


  Create CA: (Those steps are only for testing, please create CA create steps for project)

    Prepare CNF first, run the below command in windows command line.

    1. Create key and CSR file
      openssl req -new -newkey rsa:2048 -nodes -out httpboot_local.csr -keyout httpboot_local.key -subj "/C=TW/ST=Taipei/L=Taipei/O=Insyde.com/OU=OU/CN=httpboot.local" -config openssl.cnf

    2. Create CRT file by key
      openssl x509 -req -in httpboot_local.csr -signkey httpboot_local.key -out httpboot_local.crt -days 3650

    3. Create PEM file to content key and CRT files.
      type httpboot_local.key httpboot_local.crt > httpboot_local.pem

    CA

      openssl genrsa -out ca.key 2048
      openssl req -new -x509 -key ca.key -out ca.crt -config openssl.cnf
      type ca.key ca.crt > ca.pem

    sign with CA

      openssl req -new -newkey rsa:2048 -nodes -out httpboot_local.csr -keyout httpboot_local.key -subj "/C=TW/ST=Taipei/L=Taipei/O=Insyde.com/OU=OU/CN=httpboot.local" -config openssl.cnf
      openssl x509 -req -in httpboot_local.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out httpboot_localCA.crt
      type httpboot_local.key httpboot_localCA.crt > httpboot_localCA.pem

  CA management: (Via ImportCA application, project side can follow the steps in the application source code to implement management interface)

    Import:
      ImportCA --import --file CA_FILE_PATH --guid GUID_STRING

      If --guid is not exist, it will using the default GUID to import CA to DB.

    Export:
      ImportCA --export INDEX --file CA_OUTPUT_FILE_PATH

      Get INDEX via command "Import --list".

    List:
      ImportCA --list

  Download HTTPS file by RestTest application

    RestTest --device 0 --get https://httpboot.local/download/bootx64.efi

Demo nsh file:

  Environment setup:
    fs0:
    ifconfig -s eth0 dhcp
    importCA --import --file CA\ca.pem

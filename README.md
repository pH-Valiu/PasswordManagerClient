# PasswordManagerClient or Vault Shield:
Created with QT 6.6.0 and MinGW 64Bit for Windows 10+
## Main features:
 - GUI based application
 - Store and access credentials for an account:
    - username, E-Mail, password, details 
 - Edit already created accounts
 - Delete an account
 - Search for an account
 - Copy specific data of an account into your clipboard without displaying all credentials
 - Create a local backup
 - Revert to a local backup
 - Change your master password

-----------------------
## Additional stuff happening in code
 - RAM encryption of the derived master password when it is not in use
 - Automatic integrity check (computes midKeyHash for each account and compares it)
 - Automatic save on application close


-----------------------
## Interaction examples:
 - Starting Vault Shield the first time: <br>
   Enter your new master password (min. 12 characters, 1x uppercase, 1x lowercase, 1x digit, 1x special character)<br>
   ![Screenshot 2023-12-02 151536](https://github.com/pH-Valiu/PasswordManagerClient/assets/110403905/af171a38-6600-4258-9215-12e759f3ed4a)

 - Main empty window:<br>
   ![Screenshot 2023-12-02 151320](https://github.com/pH-Valiu/PasswordManagerClient/assets/110403905/89a78cb4-bd70-4a10-9407-76b7063ce49c)

 - Add a new account:<br>
   ![Screenshot 2023-12-02 151413](https://github.com/pH-Valiu/PasswordManagerClient/assets/110403905/21d2a9e2-8ca4-444f-b5d4-fe1cc405fc37)

 - Main windows after multiple accounts have been inserted and a new local backup has been manually created:<br>
   ![Screenshot 2023-12-02 151452](https://github.com/pH-Valiu/PasswordManagerClient/assets/110403905/90bf2472-4297-4e5b-b3fb-a3197b2c9f88)

 - Settings dialog:<br>
   Change master password or request an additional integrity check<br>
   ![Screenshot 2023-12-02 151507](https://github.com/pH-Valiu/PasswordManagerClient/assets/110403905/9b18e4d4-5ff5-4dbd-bd02-5db3d974b748)

 - Starting Vault Shield NOT the first time: <br>
   Enter your master password<br>
   ![Screenshot 2023-12-02 151647](https://github.com/pH-Valiu/PasswordManagerClient/assets/110403905/db1969d6-3a80-4de4-b259-9c867ec3bb86)

-----------------------
## How to get
Build it yourself via Qt or download latest release <br>
Either:
  - fullDeploy (slightly bigger, more future proof)
  - minDeploy (slightly smaller, same performance/features)


-----------------------
## How to build
Run PasswordManagerClient.pro in Qt Creator <br>
Take file VaultShield.exe and put it in a new folder <br>
Take QAESEncryption.dll and crypt32.dll from repository's library folder and put them parallel to VaultShield.exe <br>
Open a command line and execute ..\Qt\Tools\QtDesignStudio\qt6_design_studio_reduced_version\bin\windeployqt.exe in your new folder <br>
<details open>
  <summary>Make sure the folder now contains:</summary>
  <ul>
  <li>VaultShield.exe</li>
  <li>QAESEncryption.dll</li>
  <li>crypt32.dll</li>
  <li>gui:</li>
    <ul>
      <li>ico:</li>
        <ul>
           <li>all .ico files that are in repository's gui/ico/ folder aswell</li>
        </ul>
    </ul>
 <li>imageformats:</li>
    <ul>
      <li>qgif.dll</li>
      <li>qicns.dll</li>
      <li>qico.dll</li>
      <li>qjpeg.dll</li>
      <li>qpdf.dll</li>
      <li>qsvg.dll</li>
      <li>qtga.dll</li>
      <li>qtiff.dll</li>
      <li>qwbmp.dll</li>
      <li>qwebp.dll</li>
    </ul>
 <li>platforms:</li>
    <ul>
      <li>qwindows.dll</li>
    </ul>
 <li>style:</li>
    <ul>
      <li>qwindowsvistastyle.dll</li>
    </ul>
 <li>D3Dcompiler_47.dll</li>
 <li>libgcc_s-seh-1.dll</li>
 <li>libstdc++-6.dll</li>
 <li>libwinpthread-1.dll</li>
 <li>opengl32sw.dll</li>
 <li>Qt6Core.dll</li>
 <li>Qt6Gui.dll</li>
 <li>Qt6Network.dll</li>
 <li>Qt6Pdf.dll</li>
 <li>Qt6Svg.dll</li>
 <li>Qt6Test.dll</li>
 <li>Qt6Widgets.dll</li>
</details>
 

-----------------------
## How to execute
Execute VaultShield.exe

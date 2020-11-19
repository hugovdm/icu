# Obsolete: Subversion Setup for ICU Developers

This page is obsolete. Please see

*   [Source Code Access](../index.md)
*   [git for ICU Developers](../repository/gitdev/index.md)
*   [Onboarding new Contributors](../processes/onboarding-new-contributors.md)

We used Subversion until 2018-June. We are using git on GitHub since 2018-July.

~~Here are the steps for configuring access to the ICU Subversion repository:~~

## ~~1. Get ICU Server Account~~

~~A member of the ICU development team will set this up for you when you become an ICU committer, and will install your SSH key file on the server to enable secure access.~~
~~For anonymous Subversion access, see the instructions at [Using the ICU Source
Repository](http://icu-project.org/repository/index.html) for web based access,
or alternatively follow steps 5 and 7 below, skipping any step referring to ssh
setup and substituting "http://" everywhere "svn+ssh://" appears in the
instructions.~~

## ~~2. Install Putty (Windows Only)~~

> ~~Note: an alternative for Windows is to install ssh and svn clients from
> Cygwin. The setup procedure then closely follows the instructions for Unix,
> and there is no need for the Putty applications.~~

~~Putty is a Windows ssh client program that Subversion will use when communicating with the ICU Subversion server.~~
~~If you don't already have putty installed, download and install the latest full package (putty-0.60-installer.exe or later) from <http://www.chiark.greenend.org.uk/~sgtatham/putty/download.html>~~
~~After installation, add the putty install directory, typically c:\\program files\\putty, to your System Path.~~

~~Important: The putty directory really does need to be in your System Path for the rest of the setup described below to work. The installer does not do this for you.~~

## ~~3. SSH Key Generation~~

### ~~Linux / Macintosh / Unix SSH Key Generation~~

~~Run the following, after replacing *your_email_address@wherever* and *your_pass_phrase*~~

~~cd ~/.ssh~~

~~ssh-keygen -t rsa -C *your_email_address@wherever* -N *"your_pass_phrase*" -f
icu-project-key~~

~~Create the .ssh directory first if it does not already exist.~~
~~The command will generate two files, icu-project-key and icu-project-key.pub. Give the public key file to your contact person on the ICU project who is setting up committer access for you. Configuration of SSH to use the private key file is described in the next section, Configure SSH. Keep your private key private. Do not give a copy to anyone, or keep a copy in any shared directory where it could potentially be accessed by others.~~

### ~~Windows~~

1.  ~~Run the program "PUTTYgen"~~
2.  ~~Generate an "SSH-2" key (RSA or DSA) - click the Generate button. Move the
    mouse until the progress bar goes to the right.~~
3.  ~~Save the private key, you will need to find it again when you specify
    "Private Key for Authentication". It has the extension ".ppk".~~
4.  ~~Copy off the text in the area "Public Key for pasting...". It will start
    with "ssh-rsa" or "ssh-dsa".. Make sure to copy the whole thing!. Give the
    public key file to your contact person on the ICU project who is [setting up
    committer access](../processes/onboarding-new-contributors.md) for you.
    Example:: ssh-rsa
    AAAAB3NzaC1yc2EAAAABJQAAAIBmk7wb5yvneUojmDku6359pkBs2GBupv9NDxApQbz60X31I9vDaOFDJmNv/L8FnB/ieBWWfzQ74TLp0gFANx4hzKrnMiUR0O9SyJwE+3YaByx6zZD0m04O1kv0LzBLY/CNy0gOtZ76mrurxRlR42vE8kSqGVfPgOxuCBt3jblMpQ==
    rsa-key-20110907~~

## ~~4. Configure SSH~~

### ~~Linux SSH configuration~~

~~In your ssh config file, at ~/.ssh/config, add a section for the ICU server of this form~~

~~# for ICU~~
~~Host \*icu-project.org~~
~~User your-icu-user-name~~
~~IdentityFile ~/.ssh/icu-project-key~~
~~port 922~~

~~Use your ICU user name on the User line, and the name of your private key file on the IdentityFile line.~~
~~Your private key file must permissions must not allow access by others, or it wont work. The config file itself may also have that restriction.~~

~~chmod 400 icu-project-key~~

~~chmod 400 config~~

~~Test the configuration by logging into the server~~

~~$ ssh icu-project.org~~
~~Last login: Thu Oct 4 22:18:47 2007 from 216-239-45-4.go~~
~~\[heninger@v ~\]$~~

~~If you get a password prompt rather than a successful login, something is wrong with the configuration. It must be fixed before proceeding further.~~
~~If you are new to ssh setup, ask for help.~~

### ~~Windows SSH configuration~~

~~Run putty. At a command prompt,~~

~~c:> putty~~

~~A configuration dialog should open. Set up as shown below. Only the circled settings need to be changed.~~
~~Specify your own login name for icu-project.org, not mine!~~
~~Specify your private key file for source.icu-project.org.~~
~~Save the putty configuration.~~
~~Then click the "open" button. If you get a security alert regarding the server's host key not being cached in the registry, respond with YES.~~
~~Putty should connect to the server.~~
~~If you get a password prompt rather than a successful login, something is wrong in the putty setup. It must be fixed before proceeding further.~~

## ~~5. Install the Subversion command line based client~~

### ~~Linux~~

~~To check whether subversion is already installed, try the command~~

~~svn --help~~

~~For distributions using Debian package management, including Ubuntu, subversion can be installed with the command~~

~~sudo apt-get install subversion~~

~~The configuration file will be at $HOME/.subversion/config.~~

### ~~Windows~~

1.  ~~Download Windows subversion binaries. A list of binary distributions is here:~~
    ~~<http://subversion.apache.org/packages.html#windows>~~
    ~~For command line only binaries, this one works well:~~
    ~~<http://www.sliksvn.com/en/download>~~
2.  ~~Install. Subversion installs with a normal Windows installer setup wizard.~~
    svn --help~~ ~~After installation, from a command line, try
    to verify that the install worked. A pre-existing command windows may not see a new installation, if it doesn't work try opening a new command window.~~
3.  ~~Configure subversion for ssh.~~
    ~~The svn configuration file is in the Applications Data directory, and will have a path of this form:~~
    ~~\[XP\]~~
    ~~` C:\Documents and Settings\<your Windows login name>\Application Data\Subversion\config `~~
    ~~\[Windows 7\]~~
    ~~` %USERPROFILE%\AppData\Roaming\Subversion\config`~~
    ~~Using any plain text editor, add the line~~
    ~~ssh = $SVN_SSH plink.exe~~
    ~~into the \[tunnels\] section of the file, as shown below:~~

~~### Section for configuring tunnel agents.~~
~~\[tunnels\]~~
~~### Configure svn protocol tunnel schemes here. By default, only~~
~~### the 'ssh' scheme is defined. You can define other schemes to~~
~~### be used with 'svn+scheme://hostname/path' URLs. A scheme~~
~~### definition is simply a command, optionally prefixed by an~~
~~### environment variable name which can override the command if it~~
~~### is defined. The command (or environment variable) may contain~~
~~### arguments, using standard shell quoting for arguments with~~
~~### spaces. The command will be invoked as:~~
~~### <command> <hostname> svnserve -t~~
~~### (If the URL includes a username, then the hostname will be~~
~~### passed to the tunnel agent as <user>@<hostname>.) If the~~
~~### built-in ssh scheme were not predefined, it could be defined~~
~~### as:~~
~~# ssh = $SVN_SSH ssh~~
~~**ssh = $SVN_SSH plink.exe**~~
~~### If you wanted to define a new 'rsh' scheme, to be used with~~

~~Note: You can include the complete path to plink.exe here as an alternative to adding the Putty directory to your system PATH, as described in step 2. Backslash escape any spaces in the path, and use forward slashes for directory separators.~~

~~Note: with this setup, when subversion asks for "source.icu-project.org", the request will go to plink.exe, which will look for a saved putty configuration of that name ("source.icu-project.org"), and access whatever server machine that configuration indicates. There is no requirement that the saved putty configuration name match the host name that will be accessed, but it keeps things less confusing if they are the same.~~

## ~~6. Auto Properties Configuration~~

~~No configuration setup for svn auto-props is needed.~~

~~If you have configured auto-props according to to the instructions that used
to be in this section, you can leave it; the config settings will do no harm.~~

~~Subversion auto-props set the default file properties when new files are added
to the repository.~~

~~Note: Automatic setup of auto-props requires svn client software version 1.8
or newer. Check your version with the command "`svn --version`" and upgrade if
necessary.~~

~~Note to icu repository maintainers:~~

> ~~To see the auto-props settings, run the command `svn propget svn:auto-props
> http://icu-project.org/repos/icu.`~~

> ~~To change the settings, do a non-recursive checkout of the repository root,
> svn+ssh://source.icu-project.org/repos/icu, cd icu; svn propedit
> svn:auto-props . ; svn commit~~

### ~~Other SVN Configuration Settings~~

~~Modify the `global-ignores` to include the following:~~

> `~~global-ignores = *.o *.pyc *.lo *.la #*# .*.rej *.rej .*~ *~ .#* .DS_Store
> *.pbxuser *.mode1 .directory~~`

## ~~7. Test Subversion Server Access~~

~~Make a test directory and try a checkout.~~
C:\\>mkdir svntest~~
C:\\>cd svntest~~
C:\\svntest>svn co svn+ssh://source.icu-project.org/repos/icu/icu4j/trunk icu/icu4j/trunk~~
A icu/icu4j/trunk/.classpath~~
A icu/icu4j/trunk/readme.html~~
A icu/icu4j/trunk/.project~~
A icu/icu4j/trunk/preprocessor.txt~~
A icu/icu4j/trunk/unicode-license.txt~~
A icu/icu4j/trunk/license.html~~
A icu/icu4j/trunk/eclipseFragment.txt~~
A icu/icu4j/trunk/eclipseProjectMisc~~
A icu/icu4j/trunk/eclipseProjectMisc/initSrc.launch~~
A icu/icu4j/trunk/eclipseProjectMisc/normSrc.launch~~
A icu/icu4j/trunk/src~~
*\[snipped the rest\]*~~

~~Keeping the directory structure on your local machine the same as that of the
repository, back to the top level icu/ directory, provides an easy way to keep
track of what has been checked out where. This is not required, but highly
recommended.~~

## ~~8. GUI Clients~~

~~There are many GUI based Subversion clients available. Here are some that have
proven to work reasonably well.~~

~~RapidSVN, available for Windows, Macintosh and Linux, presents an interface
that is similar to that of WinCVS. <http://www.rapidsvn.org/>~~

~~TortoiseSVN, for Windows only, integrates Subversion functionality directly
into the Windows file Explorer.[
http://tortoisesvn.net/](http://tortoisesvn.net/)~~

### ~~Subversive (plugin for Eclipse)~~

~~See~~

*   [~~Eclipse Setup for Java
    Developers~~](java/eclipse-setup-for-java-developers/index.md)

#### ~~Installation~~

1.  ~~Install Eclipse 3.5~~
2.  ~~Help > Install New Software~~
3.  ~~Work with: subversive~~
    1.  ~~a pulldown menu will show, with some choices. Pick:~~
    2.  ~~Subversive - http://download.eclipse.org/technology/subversive/0.7/update-site/~~
4.  ~~Pick Subversive SVN Team Provider Plugin (Incubation)~~
5.  ~~Next, Finish, Restart Eclipse~~
6.  ~~Do above again, but Pick:~~
    1.  ~~Subversive SVN Connectors Site -
        http://community.polarion.com/projects/subversive/download/eclipse/2.0/galileo-site/~~
7.  ~~Pick Subversive SVN Connectors, expand tree~~
8.  ~~Pick only "Subversive SVN Connectors" and "SVNKit 1.3.0 Implementation" --
    deselect all other (Optional) items!~~
9.  ~~Next, Finish, Restart Eclipse~~

#### ~~Repository Setup~~

1.  ~~in Eclipse, Window > Open Perspective > Other > SVN Repository Exploring~~
2.  ~~In the SVN Repository pane, Right-Click > New > Repository Location~~
3.  ~~General Tab~~
    1.  ~~URL: svn+ssh://source.icu-project.org/repos/icu/icu4j~~
    2.  ~~User: <your user name>~~
    3.  ~~Password: <empty>~~
4.  ~~SSH Settings Tab~~
    1.  ~~Port: 922~~
    2.  ~~Private Key (choose)~~
    3.  ~~File: <your private key file>~~
    4.  ~~Passphrase: <your private key passphrase>~~
5.  ~~Finish~~

#### ~~Loading ICU4J~~

*   ~~See [Subversive SVN plugin](../design/icu4j-development-with-eclipse.md)~~

## ~~9. Using the ICU Repository~~

~~Refer back to the main [ICU Repository](../index.md) page.~~

# Obsolete: Onboarding new Contributors

## Administrative

1.  Ensure the user has signed/is covered by the
    [CLA](https://ssl.icu-project.org/trac/#cla)
2.  Ensure the user is listed in the "**Unicode ICU-TC Committers**🔒" document
    (private) with their name, email, and **account id**

## Trac Access

1.  On the server, create the password entry for **username** (will prompt for
    the new password)

    <pre><code># ssh root@ssl.icu-project.org
    # cd /home/icutrac/conf
    # cp trac.htpasswd _trac.htpasswd
    # htpasswd trac.htpasswd <i><b>username</b></i>
    </code></pre>

    <pre><code>New Password: <i>&lt;</i><i><b>type the new password here&gt;</b></i>
    </code></pre>

2.  Restart the server

    ```none
    # apachectl graceful
    ```

3.  (**important**) log into trac **as** the user in order to create the
    account: Visit <http://bugs.icu-project.org/trac/login> from *another
    browser*
4.  verify that you are logged in as the new user
5.  Now, grant the user permissions in trac itself:
    1.  <http://bugs.icu-project.org/trac/admin/general/perm> ( must be logged
        in as yourself )
    2.  In the box ‘Add Subject to Group’: Subject: **username**Group:developer
    3.  Click **Add**

## SSH Access

1.  Obtain the developer's SSH public key. See [Subversion Setup for ICU
    Developers](../setup/subversion.md) - it should begin with "`ssh-rsa …`" or
    "`ssh-dsa …`"
2.  On the server, # `adduser **username** -c '**User Name**' -g src -G htdocs ;
    mkdir ~**username**/.ssh`
3.  Copy the user’s key to `~`**username**`/.ssh/authorized_keys`
4.  Fix permissions: `chown -R **username**.src ~**username**/ && chmod -R
    u=rw,go= ~**username**/ && chmod u+rx ~**username**/ ~**username**/.ssh`
5.  The user should be able to login via the key but not via password.

## How to contribute

[How to contribute to ICU](contribute.md)

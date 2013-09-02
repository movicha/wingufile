# Server Release Package

1. libwingurpc
    cd libwingurpc;
    CFLAGS="-O2" configure --prefix=$dest
    make install
2. Ccnet
    cd ccnet;
    CFLAGS="-O2" ./configure --enable-server-pkg --prefix=$dest
    make install
3. Wingufile
    cd wingufile; 
    CFLAGS="-O2" configure --enable-server-pkg --prefix=$dest
    make install
4. copy shared libraries
    scripts/cp-shared-lib.py $dest/lib
5. strip libs/executables   
    python do-strip.py
6. Update winguhub
    cd winguhub
    git fetch origin
    git checkout release
    git rebase origin/master

7. Pack
    ./pack-server.sh 1.0.0
    
DONE!   
    
    
    

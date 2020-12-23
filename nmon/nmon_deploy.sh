#!/bin/sh

scriptDir="`readlink -f $0 | xargs dirname`"
outputLog="$scriptDir/output.log"
propertyFile="$scriptDir/deploy.properties"
nmonTools="$scriptDir/nmon.tar"
modifyCron="true"
dependenciesFailed="false"
mode="deploy"
dependencies="netstat telnet cron"

nmonArgs="-f -T"
nmonLogDir=""

senderArgs="" 
senderKey=""
senderProtocol=""

systemSender=""
systemServers=""
systemUser=""
systemKey=""
systemDirectory=""
systemUserSudo=""
systemKeySudo=""
systemPwd=""
systemSudoPwd=""

cronShedule=""
testVar="test"

while getopts "ndc:i" opt
do
    case $opt in
        c) propertyFile="$OPTARG";;
        n) modifyCron="false";;
        d) mode="reset";;
        i) mode="install";;
        *) echo "Available arguments: "
           echo "-c [propertyFilePath] : set path to property file. Default path: $propertyFile."
           echo "-n                    : do not modify cron. Will useful to update nmon or sender script."
           echo "-d                    : comment out cron jobs which run from deploy.directory"
           echo "-i                    : install mode. Check and install dependencies only. Requires the sudo credendials."
           exit 1
           ;;
    esac
done

if [ ! -f "$propertyFile" ]; then
    echo "Property file not found. ERROR."
    exit 1
fi

readProperties () {
    echo "Parsing property file ..."
    senderServer=""
    cronHour=""
    cronMinute=""
    while read line; do
        if [ `echo "$line" | grep -q "^#.*$"` ]; then
            continue
        fi
        key="`echo $line | awk -F = '{print $1}' | xargs`" 
        value="`echo $line | awk -F = '{print $2}' | xargs`" 
        case "$key" in
            "nmon.s")
                validValue="`echo "$value" | sed s/[^0-9]//g`"
                if [ "$validValue" != "$value" -o "$validValue" = "" ]; then
                    echo "Invalid property: $key. Only digit value available."
                else
                    nmonArgs="$nmonArgs -s $value"
                    senderArgs="$senderArgs -s $value"
                fi
                ;;
            "nmon.c")
                validValue="`echo "$value" | sed s/[^0-9]//g`"
                if [ "$validValue" != "$value" -o "$validValue" = "" ]; then
                    echo "Invalid property: $key. Only digit value available."
                else
                    nmonArgs="$nmonArgs -c $value"
                    senderArgs="$senderArgs -c $value"
                fi
                ;;
            "nmon.cron.hour")
                validValue="`echo "$value" | sed s/[^0-9]//g`"
                if [ "$validValue" != "$value" -o "$validValue" = "" ]; then
                    echo "Invalid property: $key. Only digit value available."
                else
                    cronHour="$value"
                fi
                ;;
            "nmon.cron.minute")
                validValue="`echo "$value" | sed s/[^0-9]//g`"
                if [ "$validValue" != "$value" -o "$validValue" = "" ]; then
                    echo "Invalid property: $key. Only digit value available."
                else
                    cronMinute="$value"
                fi
                ;;
            "atsd.port")
                validValue="`echo "$value" | sed s/[^0-9]//g`"
                if [ "$validValue" != "$value" -o "$validValue" = "" ]; then
                    echo "Invalid property: $key. Only digit value available."
                else
                    senderArgs="$senderArgs -p $value"
                fi
                ;;
            "atsd.hostname")
                senderServer="$value"
                ;;
            "atsd.user")
                senderArgs="$senderArgs -u $value"
                ;;
            "atsd.key")
                if [ ! -f "$value" ]; then
                    echo "ssh key file to send nmon data not found. ERROR."
                    exit 1
                else
                    senderKey="`readlink -f $value`"
                fi
                ;;
            "atsd.protocol" )
                senderProtocol=$value
                if [ "$value" != "telnet" -a "$value" != "ssh" ]; then
                    echo "atsd.protocol has invalid value. Available values: telnet, ssh. ERROR."
                    exit 1
                else
                    if [ "$value" = "telnet" ]; then
                        systemSender="./nmon_sender_telnet.sh"
                    elif [ "$value" = "ssh" ]; then
                        systemSender="./nmon_sender_ssh.sh"
                    fi
                fi
                if [ ! -f "$systemSender" ]; then
                    echo "Can not find sender-script : $systemSender to user protocol : $value. Please check all script to exist. ERROR."
                    exit 1
                else
                    systemSender="`readlink -f $systemSender`"
                fi
                ;;

            "deploy.target")
                systemServers="$systemServers $value"
                ;;
            "deploy.user")
                systemUser="$value"
                ;;
            "deploy.sudo.user")
                systemUserSudo="$value"
                ;;
            "deploy.key")
                if [ ! -f "$value" ]; then
                    echo "ssh key file to deploy nmon files not found. ERROR."
                    exit 1
                else
                    systemKey="`readlink -f $value`"
                fi
                ;;
            "deploy.sudo.key")
                if [ ! -f "$value" ]; then
                    echo "ssh key file to install nmon dependencies not found. ERROR."
                    exit 1
                else
                    systemKeySudo="`readlink -f $value`"
                fi
                ;;
            "deploy.password")
                systemPwd="$value"
                if [ "$value" != "" ]; then
                    if ! which sshpass; then
                        echo "Can not find sshpass utility, which is required to use password for ssh connection."
                        echo "Please install sshpass utility or use ssh-keys to continue."
                        exit 1
                    fi
                fi
                ;;
            "deploy.sudo.password")
                systemSudoPwd="$value"
                if [ "$value" != "" ]; then
                    if ! which sshpass; then
                        echo "Can not find sshpass utility, which is required to use password for ssh connection."
                        echo "Please install sshpass utility or use ssh-keys to continue."
                        exit 1
                    fi
                fi
                ;;
            "deploy.nmon-binary")
                if [ ! -f "$value" ]; then
                    echo "file in property $key does not exist: $value."
                    exit 1
                else
                    systemNmon="`readlink -f $value`"
                fi
                ;;
            "deploy.directory.log")
                nmonLogDir="$value"
                ;;
            "deploy.directory")
                systemDirectory="$value"
                nmonLogDir="$value/nmon_logs"
                nmonArgs="$nmonArgs -m $value/nmon_logs/"
                senderArgs="$senderArgs -m $value/nmon_logs/"
                ;;
        esac
    done < $propertyFile

    [ "$systemNmon" = "" ] && echo "Nmon binary file is not set ( system.nmon property ). ERROR." && exit 1
    [ "$systemSender" = "" ] && echo "Sender script file is not set ( system.nmon property ). ERROR." && exit 1
    if [ "$senderKey" = "" ]; then
        senderArgs="$senderServer$senderArgs >> $systemDirectory/full.log 2>&1"
    else
        senderArgs="$senderServer$senderArgs -i $systemDirectory/`basename $senderKey` >> $systemDirectory/full.log 2>&1"
    fi
    cronShedule="$cronMinute $cronHour * * *"
    echo "Parsinig finished."
}

showParsed () {
    echo ""
    echo "nmonArgs: $nmonArgs"
    echo "scriptArgs: $senderArgs"
    echo "atsdKey: $senderKey"
    echo "protocol: $senderProtocol"
    echo "deployServers: $systemServers"
    echo "deployUser: $systemUser"
    echo "deployKey: $systemKey"
    echo "deployPwd: $systemPwd"
    echo "deploySudoUser: $systemUserSudo"
    echo "deploySudoKey: $systemKeySudo"
    echo "deploySudoPassword: $systemSudoPwd"
    echo "deployDirectory: $systemDirectory"
    echo "cronShedule: $cronShedule"
    echo "nmon: $systemNmon"
    echo "sender-script: $systemSender ( based on protocol: $senderProtocol )"
    echo ""
}

tarFiles () {
    rm -rf $scriptDir/tarTmp 
    mkdir $scriptDir/tarTmp
    [ "$senderKey" = "" ] || cp $senderKey $scriptDir/tarTmp/
    cp $systemNmon $scriptDir/tarTmp/
    cp $systemSender $scriptDir/tarTmp/
    cd $scriptDir/tarTmp
    tar -cvf $nmonTools * >/dev/null >>$outputLog 2>&1
    cd - >/dev/null >>$outputLog 2>&1
    rm -rf $scriptDir/tarTmp
}

configureServers () {
    echo "stage: deploy files ..."
    echo ""
    nmon="`basename $systemNmon`"
    sender="`basename $systemSender`"
    [ "$senderKey" = "" ] && key="" || key="`basename $senderKey`"
    for server in $systemServers; do
        addr="`echo $server | awk -F : '{print $1}'`" 
        port="`echo $server | awk -F : '{print $2}'`" 
        [ "$port" = "" ] && port=22
        echo "Working with server: $addr, port: $port"
        echo "Configure server ..."
        if [ "$systemPwd" = "" ]; then 
            ssh -o "StrictHostKeyChecking no" -i $systemKey -p $port $systemUser@$addr "mkdir -p $nmonLogDir; [ ! -f /home/$systemUser/.cronDefault ] && crontab -l > /home/$systemUser/.cronDefault"  >>$outputLog 2>&1 #create cron backup if no exist and required directories.
            scp -i $systemKey -P $port $nmonTools $systemUser@$addr:$systemDirectory/ >>$outputLog 2>&1 #copy archive to server
        else
            sshpass -p "$systemPwd" ssh -o "StrictHostKeyChecking no" -p $port $systemUser@$addr "mkdir -p $nmonLogDir; [ ! -f /home/$systemUser/.cronDefault ] && crontab -l > /home/$systemUser/.cronDefault"  >>$outputLog 2>&1 #create cron backup if no exist and required directories.
            ec=$?
            if [ $ec -eq 5 -o $ec -eq 255 ]; then
                echo "Can not connect to remote server: $addr:$port with current credentials:"
                echo "user: $systemUser"
                echo "password: $systemPwd"
                echo ""
                continue
            fi
            sshpass -p "$systemPwd" scp -P $port $nmonTools $systemUser@$addr:$systemDirectory/ >>$outputLog 2>&1 #copy archive to server
        fi

        echo "Archive copied."

        if [ "$modifyCron" = "true" ]; then
            echo "Cron will be modified." 
            if [ "$systemPwd" = "" ]; then 
                ssh -o "StrictHostKeyChecking no" -i $systemKey -p $port $systemUser@$addr "cd $systemDirectory && tar -xf nmon.tar; crontab -l > tmpcron; echo \"$cronShedule $systemDirectory/$nmon $nmonArgs\" >>tmpcron && echo \"$cronShedule $systemDirectory/$sender $senderArgs\" >>tmpcron; echo \"\" >>tmpcron; crontab tmpcron" >>$outputLog 2>&1 #extracting files, modify cron
            else
                sshpass -p "$systemPwd" ssh -o "StrictHostKeyChecking no" -p $port $systemUser@$addr "cd $systemDirectory && tar -xf nmon.tar; crontab -l > tmpcron; echo \"$cronShedule $systemDirectory/$nmon $nmonArgs\" >>tmpcron && echo \"$cronShedule $systemDirectory/$sender $senderArgs\" >>tmpcron; echo \"\" >>tmpcron; crontab tmpcron" >>$outputLog 2>&1 #extracting files, modify cron
            fi
        else
            echo "Cron will not be modified ( -n )."
            if [ "$systemPwd" = "" ]; then 
                ssh -o "StrictHostKeyChecking no" -i $systemKey -p $port $systemUser@$addr "cd $systemDirectory && tar -xf nmon.tar" >>$outputLog 2>&1 #just extracting files ( update mode )
            else
                sshpass -p "$systemPwd" ssh -o "StrictHostKeyChecking no" -p $port $systemUser@$addr "cd $systemDirectory && tar -xf nmon.tar" >>$outputLog 2>&1 #just extracting files ( update mode )
            fi
        fi
        echo "Starting nmon ..."
        ec=0
        if [ "$systemPwd" = "" ]; then 
            ssh -o "StrictHostKeyChecking no" -i $systemKey -p $port $systemUser@$addr "if [ -z \"\`ps -ef | grep \"$systemDirectory/$nmon\" | grep -v \"grep\"\`\" ]; then eval \"\`echo '$systemDirectory/$nmon $nmonArgs' | sed -r s/-c\ [0-9]+/\-c\ `expr 1439 - \( \`date +%H\` \* 60 + \`date +%M\` \)`/\`\"; eval \"\`echo '$systemDirectory/$sender $senderArgs' | sed -r s/-c\ [0-9]+/\-c\ `expr 1439 - \( \`date +%H\` \* 60 + \`date +%M\` \)`/\` & \"; exit 0; else exit 2; fi" >>$outputLog 2>&1 #extracting files, modify cron
            ec=$?
        else
            sshpass -p "$systemPwd" ssh -o "StrictHostKeyChecking no" -p $port $systemUser@$addr "if [ -z \"\`ps -ef | grep $systemDirectory/$nmon | grep -v \"grep\"\`\" ]; then eval \"\`echo '$systemDirectory/$nmon $nmonArgs' | sed -r s/-c\ [0-9]+/\-c\ `expr 1439 - \( \`date +%H\` \* 60 + \`date +%M\` \)`/\`\"; eval \"\`echo '$systemDirectory/$sender $senderArgs' | sed -r s/-c\ [0-9]+/\-c\ `expr 1439 - \( \`date +%H\` \* 60 + \`date +%M\` \)`/\` &\"; exit 0; else exit 2; fi" >>$outputLog 2>&1 #extracting files, modify cron
            ec=$?
        fi
        if [ $ec -eq 2 ]; then
            echo "nmon is already running."
        fi
        echo "Nmon deployed."
        echo ""
    done
}

resetServers () {
    for server in $systemServers; do
        addr="`echo $server | awk -F : '{print $1}'`" 
        port="`echo $server | awk -F : '{print $2}'`" 
        [ "$port" = "" ] && port=22
        echo "Working with server: $addr, port: $port"
        preparedDirectory="`echo "$systemDirectory" |sed "s/\\//\\\\\\\\\\\\//g"`"
        if [ "$systemPwd" = "" ]; then 
            #ssh -o "StrictHostKeyChecking no" -i $systemKey -p $port $systemUser@$addr "rm -rf $systemDirectory; if [ -f /home/$systemUser/.cronDefault ]; then crontab /home/$systemUser/.cronDefault; exit 0; else exit 1; fi;" >>$outputLog 2>&1
            ssh -o "StrictHostKeyChecking no" -i $systemKey -p $port $systemUser@$addr "crontab -l | sed -r '/^[0-9\ \*]*$preparedDirectory.*/ s/^/#/' | crontab -" >>$outputLog 2>&1
        else
            sshpass -p "$systemPwd" ssh -o "StrictHostKeyChecking no" -p $port $systemUser@$addr "crontab -l | sed -r '/^[0-9\ \*]*$preparedDirectory.*/ s/^/#/' | crontab -" >>$outputLog 2>&1
            #sshpass -p "$systemPwd" ssh -o "StrictHostKeyChecking no" -i $systemKey -p $port $systemUser@$addr "crontab -l | sed -r '/^[0-9\ \*]*\$preparedDirectory.*/ s/^/#/' | crontab -" >>$outputLog 2>&1
        fi
        ec=$?
        if [ $ec -eq 0 ]; then
            echo "Jobs commented out."
        elif [ $ec -eq 5 -o $ec -eq 255 ]; then
            echo "Can not connect to remote server: $addr:$port with current credentials:"
            echo "user: $systemUser"
            echo "password: $systemPwd"
        else
            echo "Directory clean up, but default nmon file was not found to reset cron. Server reset."
        fi
        echo ""
    done
}

#return 0 if dependencies satisfied, else 1
checkDependencies () {
    echo "stage: Checking dependencies ..."
    echo ""
    for server in $systemServers; do
        addr="`echo $server | awk -F : '{print $1}'`" 
        port="`echo $server | awk -F : '{print $2}'`" 
        [ "$port" = "" ] && port=22
        echo "Working with server: $addr, port: $port"
        if [ "$systemPwd" = "" ]; then
            ssh -o "StrictHostKeyChecking no" -i $systemKey -p $port $systemUser@$addr "for util in $dependencies; do if ! which \$util >/dev/null 2>&1; then if [ \"\$util\" != \"cron\" ]; then exit 1; fi; fi; done; if [ \"\`ps -ef | grep cron | grep -v \"grep\"\`\" = \"\" ]; then exit 2; fi; if [ \"\`ps -ef | grep -i "nmon[[:space:]]" | grep -v \"\$\$\" |grep -v \"grep\"\`\" != \"\" ]; then exit 3; fi; if grep -qi \"suse\" /proc/version; then if [ ! -f /lib64/libtinfo.so.5 ]; then exit 4; fi; fi; exit 0" >>$outputLog 2>&1
        else
            sshpass -p "$systemPwd" ssh -o "StrictHostKeyChecking no" -p $port $systemUser@$addr "for util in $dependencies; do if ! which \$util >/dev/null 2>&1; then if [ \"\$util\" != \"cron\" ]; then exit 1; fi; fi; done; if [ \"\`ps -ef | grep cron | grep -v \"grep\"\`\" = \"\" ]; then exit 2; fi; if [ \"\`ps -ef | grep -i "nmon[[:space:]]" | grep -v \"\$\$\" |grep -v \"grep\"\`\" != \"\" ]; then exit 3; fi; if grep -qi \"suse\" /proc/version; then if [ ! -f /lib64/libtinfo.so.5 ]; then exit 4; fi; fi; exit 0" >>$outputLog 2>&1
        fi
        ec=$?
        echo -n "$addr:$port "
        if [ $ec -eq 1 ]; then
            echo "One of the following dependencies: [ $dependencies ] is not installed."
            echo "$addr:$port Please install dependencies manually or run script in installation mode ( -i ) with sudo user credentials ( system.user.sudo & system.key.sudo )"
            dependenciesFailed="true"
        elif [ $ec -eq 2 ]; then
            echo "Cron daemon is not running ( or is not installed )."
            echo "$addr:$port Run or install&run it before procceeding ( you are able to run $0 -i to fix it )."
            dependenciesFailed="true"
        elif [ $ec -eq 3 ]; then
            echo "nmon already running on current machine. Deploying to this machine will not continue."
            dependenciesFailed="true"
        elif [ $ec -eq 4 ]; then
            echo "libtinfo.so.5 library does not exist. To fix it, you can run the follow command on target machine: sudo ln -s /lib64/libncurses.so.5.6 /lib64/libtinfo.so.5"
            dependenciesFailed="true"
        elif [ $ec -eq 5 -o $ec -eq 255 ]; then
            echo "Can not connect to remote server: $addr:$port with current credentials:"
            echo "user: $systemUser"
            echo "password: $systemPwd"
            dependenciesFailed="true"
        elif [ $ec -eq 0 ]; then
            echo "is OK"
        else
            echo "Unrecognized error. Can not check dependencies."
            dependenciesFailed="true"
        fi
        echo ""
    done

    [ "$dependenciesFailed" = "true" ] && return 1 || return 0
}


installDependencies () {
    for server in $systemServers; do
        addr="`echo $server | awk -F : '{print $1}'`" 
        port="`echo $server | awk -F : '{print $2}'`" 
        [ "$port" = "" ] && port=22
        echo "Installation of dependencies: Working with server: $addr, port: $port"
        if [ "$systemSudoPwd" = "" ]; then
            ssh -o "StrictHostKeyChecking no" -i $systemKeySudo -p $port $systemUserSudo@$addr "which yum && yum install -y cronie telnet net-tools || which zypper && zypper -n install cron telnet net-tools || which apt-get && apt-get install -y cron telnet net-tools; if [ \"\`ps -ef | grep cron | grep -v \"grep\"\`\" = \"\" ]; then cron; fi; for util in $dependencies; do if ! which \$util >/dev/null 2>&1; then if [ \"\$util\" != \"cron\" ]; then exit 1; fi; fi; done; if [ \"\`ps -ef | grep cron | grep -v \"grep\"\`\" = \"\" ]; then exit 2; fi; exit 0" >>$outputLog 2>&1
        else
            sshpass -p "$systemSudoPwd" ssh -o "StrictHostKeyChecking no" -p $port $systemUserSudo@$addr "which yum && yum install -y cronie telnet net-tools || which zypper && zypper -n install cron telnet net-tools || which apt-get && apt-get install -y cron telnet net-tools; if [ \"\`ps -ef | grep cron | grep -v \"grep\"\`\" = \"\" ]; then cron; fi; for util in $dependencies; do if ! which \$util >/dev/null 2>&1; then if [ \"\$util\" != \"cron\" ]; then exit 1; fi; fi; done; if [ \"\`ps -ef | grep cron | grep -v \"grep\"\`\" = \"\" ]; then exit 2; fi; exit 0" >>$outputLog 2>&1
        fi
        ec=$?
        if [ $ec -eq 1 ]; then
            echo "Cannot install dependencies on current server. Please install them manually."
        elif [ $ec -eq 2 ]; then
            echo "Cannot run cron daemon ( or install it ) automaticly. Please start it manually before procceeding."
        elif [ $ec -eq 5 -o $ec -eq 255 ]; then
            echo "Can not connect to remote server with current credentials:"
            echo "user: $systemUserSudo"
            echo "password: $systemSudoPwd"
        else
            echo "Dependencies on server $server have been installed."
        fi
        echo ""
    done
    exit 0

}

readProperties
showParsed
echo "Current mode: $mode"
echo ""
case "$mode" in
    "reset")
        [ "$systemUser" = "" ] && echo "User not defined. ERROR." && exit 1
        [ "$systemKey" = "" -a "$systemPwd" = "" ] && echo "$systemUser ssh-key and password for user not defined. ERROR." && exit 1
        resetServers
        exit 0
        ;;
    "install")
        [ "$systemUserSudo" = "" ] && echo "Sudo user not defined. ERROR." && exit 1
        [ "$systemKeySudo" = "" -a "$systemSudoPwd" = "" ] && echo "Sudo ssh-key and password not defined. ERROR." && exit 1
        installDependencies
        exit 0
        ;;
    *)
        [ "$systemUser" = "" ] && echo "User not defined. ERROR." && exit 1
        [ "$systemKey" = ""  -a "$systemPwd" = "" ] && echo "ssh-key not defined. ERROR." && exit 1
        tarFiles
        checkDependencies
        if [ $? -eq 0 ]; then
            configureServers
        else
            echo "Some servers do not have the required dependencies. Install dependencies before procceeding."
        fi
        exit 0
        ;;
esac



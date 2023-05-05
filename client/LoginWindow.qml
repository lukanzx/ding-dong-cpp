import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.0
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.3
import "./js/config_utils.js" as ConfigUtils

Window {
    property string usrID: Config.read("myID","")
    property string usrPSW: ""
    property string usrHead: Config.read("myHead", "https://c-ssl.duitang.com/uploads/blog/201408/15/20140815095903_ttcnF.jpeg")     //登陆时若本地就有头像自动获取；否则使用自带默认头像（类qq的企鹅）
    property string usrName: ""
    property string usrPassword: ""

    id: loginWindows
    width: 248
    height: 400
    visible: true
    title: qsTr("登陆")
    flags: Qt.Window

    RegisterWindow{id: registerWindows; visible: false;}
    MainWindow{id: mainWindows; visible: false;}

    signal loginSignal(int usrID,string usrPSW)

    MessageDialog
    {
            id: wrongLogin
            title: "提示"
            icon: StandardIcon.Warning
            text: "账号或密码错误"
            standardButtons: StandardButton.Cancel
    }
    MessageDialog
    {
            id: notNumInputText
            title: "提示"
            icon: StandardIcon.Warning
            text: "请输入数字"
            standardButtons: StandardButton.Cancel
    }

    function loginBack(v)
    {
        if(1 == v)
        {
            Config.write("myID", usrID)
            ClientInfo.id = Number(usrID)
            ClientInfo.avatar = usrHead
            mainWindows.show()
            loginWindows.visible = false
        }
        else
        {
            wrongLogin.open()
        }
    }

    Rectangle {
        id: img
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -125
        width: 100
        height: 100
        radius: width/2
        color: "black"

        Image {
            id: _image
            smooth: true
            visible: false
            anchors.fill: parent
            source: usrHead
            antialiasing: true
        }
        Rectangle {
            id: _mask
            anchors.fill: parent
            radius: width/2
            visible: false
            antialiasing: true
            smooth: true
        }
        OpacityMask {
            id:mask_image
            anchors.fill: _image
            source: _image
            maskSource: _mask
            visible: true
            antialiasing: true
        }
    }

    TextField
    {
        id: inputID
        width:200
        height: 50
        y:150
        x:20
        leftPadding: 4
        placeholderText: qsTr("<center>账号</center>")
        text: usrID
    }
    TextField
    {
        id: inputPSW
        width:200
        height: 50
        y:200
        x:20
        leftPadding: 4
        echoMode: TextInput.Password
        placeholderText: qsTr("<center>密码</center>")
        text: usrPassword
    }

    Button
    {
        id: sendMsg
        objectName: "loginButton"
        anchors.centerIn: parent
        anchors.verticalCenterOffset: 90
        width: 40
        height: 40
        icon.source: sendMsg.pressed? "qrc:/images/rightArrow1.png":
                                     ("qrc:/images/rightArrow.png")
        onClicked:
        {
            if(isNaN(inputID.text))
            {
                notNumInputText.open()
                inputID.text = ""
            }
            else
            {
                usrID = Number(inputID.text)
                usrPSW = inputPSW.text
                //发送消息
                console.log(usrPSW)
                console.log("登录")
                loginSignal(usrID,usrPSW)
            }
        }
    }

    ToolButton
    {
        width:50
        height: 30
        objectName: "registerButton"
        anchors.centerIn: parent
        anchors.verticalCenterOffset: 180
        highlighted: true
        text: qsTr("注册")
        onClicked:
        {
            registerWindows.show()
            loginWindows.hide()
        }
    }

}

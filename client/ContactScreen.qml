import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.3

import "./components"

Rectangle {
    id: rectangle
    radius: 4
    objectName: "contactScreen"

    signal messageWithConact(int id, string name, string avatar)
    signal messageWithGroup(int id, string name, string avatar)

    signal requestContactSignal()
    signal requestGroupSignal()

    signal createGroupSignal(var mId)

    function setContacts(contacts){
        contactListModel.clear()
        for(let each of contacts){
            contactListModel.append(each)
        }
    }

    function appendContact(data){
        contactListModel.append(data)
    }

    function setGroups(groups){
        groupListModel.clear()
        for(let each of groups){
            groupListModel.append(each)
        }
    }

    function appendGroup(data){
        groupListModel.append(data)
    }

    function sendRequestDataSignal(){
        console.log("获取通讯录信息……")
        requestContactSignal()
        requestGroupSignal()
    }

    AddFriendWindow{
        id : addFriendWindow; visible: false;
        onPassSignal: (uid, avatar, name, isPerson) => {
            if(isPerson) appendContact({"userId":uid,"userName":name,"avatar":avatar})
            else appendGroup({"userId":uid,"userName":name,"avatar":avatar})
        }
    }
    CreateGroup{
        id:createGroup;
        visible: false;
        objectName: "createGroup"
    }

    PersonalInf{id: personalInf; visible: false}
    GroupInf{id:groupInf; visible:false}

    signal infSignal(int uid, bool isGroup)

    function personalInfBack(data)
    {

        personalInf.personalSaying = data.signature
        personalInf.sex_num = Number(data.gender)
        personalInf.birthday = data.birth
        personalInf.areaFrom = data.area
        personalInf.edu = data.education
        personalInf.personalHead = data.icon
        personalInf.personalName = data.name
        personalInf.personalID = data.id

        personalInf.visible = true
    }
    function openPInfoWindow(data,){

        console.log("emit")
        infSignal(data["userId"], false)

        personalInf.isMe = false
        personalInf.personalHead = data["avatar"]
        personalInf.personalName = data["userName"]
        personalInf.personalID   = data["userId"]

    }

    function groupInfBack(data)
    {
        groupInf.groupNotic = data.groupNotice
        groupInf.groupSummary = data.groupSummary
        groupInf.isOwner = data.isOwner
    }


    function openGInfoWindow(data){

        infSignal(data["userId"],true)

        groupInf.groupHead = data["avatar"]
        groupInf.groupName = data["userName"]
        groupInf.groupID   = data["userId"]

        groupInf.visible = true
    }

    function createGroupIsSuccess(v)
    {
        (0 != v)? (createGroupSuccess.open()):(createGroupError.open())
    }

    MessageDialog
    {
            id: createGroupSuccess
            title: "提示"
            icon: StandardIcon.Warning
            text: "创建群聊成功"
            standardButtons: StandardButton.Cancel
    }
    MessageDialog
    {
            id: createGroupError
            title: "提示"
            icon: StandardIcon.Warning
            text: "创建群聊失败"
            standardButtons: StandardButton.Cancel
    }

    ListModel {
        id: contactListModel
        ListElement {
            userId: 100001
            userName: "张三"
            avatar: "https://www.com8.cn/wp-content/uploads/2020/08/20200823052248-5f41fd28d49e4.jpg"
            isGroup: false
        }
        ListElement {
            userId: 1
            userName: "张三"
            avatar: "https://www.com8.cn/wp-content/uploads/2020/08/20200823052248-5f41fd28d49e4.jpg"
            isGroup: false
        }
        ListElement {
            userId: 1
            userName: "张三"
            avatar: "https://www.com8.cn/wp-content/uploads/2020/08/20200823052248-5f41fd28d49e4.jpg"
            isGroup: false
        }
        ListElement {
            userId: 100001
            userName: "张三"
            avatar: "https://www.com8.cn/wp-content/uploads/2020/08/20200823052248-5f41fd28d49e4.jpg"
            isGroup: false
        }
        ListElement {
            userId: 1
            userName: "张三"
            avatar: "https://www.com8.cn/wp-content/uploads/2020/08/20200823052248-5f41fd28d49e4.jpg"
            isGroup: false
        }
        ListElement {
            userId: 1
            userName: "张三"
            avatar: "https://www.com8.cn/wp-content/uploads/2020/08/20200823052248-5f41fd28d49e4.jpg"
            isGroup: false
        }
    }

    ListModel {
        id: groupListModel
        ListElement {
            userId: 1
            userName: "张三"
            avatar: "https://www.com8.cn/wp-content/uploads/2020/08/20200823052248-5f41fd28d49e4.jpg"
            isGroup: true
        }
        ListElement {
            userId: 1
            userName: "张三"
            avatar: "https://www.com8.cn/wp-content/uploads/2020/08/20200823052248-5f41fd28d49e4.jpg"
            isGroup: true
        }
        ListElement {
            userId: 1
            userName: "张三"
            avatar: "https://www.com8.cn/wp-content/uploads/2020/08/20200823052248-5f41fd28d49e4.jpg"
            isGroup: true
        }
    }

    Component {
        id: contactListItem
        RowLayout
        {

            width: 240
            height: 64
            RoundImage
            {
                img_src: avatar
                width: 40
                height: width
                color: "black"
                Layout.alignment: Qt.AlignLeft

                MouseArea
                {
                    anchors.fill: parent
                    hoverEnabled: true
                    onHoveredChanged:
                    {
                        cursorShape = Qt.PointingHandCursor
                    }
                    acceptedButtons: Qt.LeftButton
                    onClicked:
                    {
                        if(!isGroup&&Number(userId)<600000)
                        {
                            openPInfoWindow(contactListModel.get(index))
                        }
                        else
                        {
                            openGInfoWindow(groupListModel.get(index))
                        }
                    }

                }
            }
            Text
            {
                Layout.leftMargin: 5
                text: userName + qsTr("<font color=\"#54b4ef\">(") + userId + qsTr(")</font>")
                elide: Text.ElideRight
                Layout.alignment: Qt.AlignLeft
            }
            Button
            {
                id: contactButton
                width: 20
                height: 20
                background: Item{
                    opacity:1
                }
                onClicked: {

                    if(100000 <= userId && userId <= 599999){
                        messageWithConact(userId, userName, avatar);
                    }else messageWithGroup(userId, userName, avatar);
                }

                Layout.alignment: Qt.AlignRight
                icon.source: "./images/icon_chat.png"
                icon.color: contactButton.hovered ? Material.accent : "transparent"
            }
        }

    }

    ColumnLayout {
        anchors.margins: 12
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 400
        spacing: 0
        anchors.left: parent.left
        anchors.leftMargin: 12

        Label {
            text: "好友"
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            Layout.leftMargin: 4
            Layout.bottomMargin: 4
        }

        ListView {
            id: contactListView
            model: contactListModel
            spacing: 8
            focus: true
            Layout.fillWidth: true
            height: Math.min(contactListView.contentHeight, 280)
            //            Layout.fillHeight: true
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            clip: true
            delegate: contactListItem
        }

        Label {
            text: "群聊"
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            Layout.leftMargin: 4
            Layout.bottomMargin: 4
        }

        ListView {
            id: groupListView
            model: groupListModel
            spacing: 8
            focus: true
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            clip: true
            delegate: contactListItem

        }
        RowLayout
        {
            Button {
                height: 56; width: 56
                background: Rectangle {
                    color: "transparent"
                }
                icon.source: "../images/addFriend.png"
                icon.width: 32
                icon.height: 32

                Layout.alignment: Qt.AlignLeft
                onClicked: {
                    addFriendWindow.show()
                }
            }
            Button {
                height: 56; width: 56
                background: Rectangle {
                    color: "transparent"
                }
                icon.source: "../images/addGroup.png"
                icon.width: 25
                icon.height: 25

                Layout.alignment: Qt.AlignLeft
                onClicked: {
                    createGroupSignal(1)
                }
            }
        }

    }
}

import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.0
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.5

Window {
    id: groupPage
    width: 338
    height: 506
    visible: false

    property string groupHead: "https://c-ssl.dtstatic.com/uploads/item/202007/23/20200723100019_v4nJm.thumb.1000_0.jpeg"
    property string pBackground: "https://c-ssl.dtstatic.com/uploads/item/202004/25/20200425235603_ybgrj.thumb.1000_0.jpg"
    property string groupName: "小组"
    property int groupID: 12345
    property string groupNotic: "hello"
    property string groupSummary: "小组"
    property bool setFlag: true

    property bool isOwner: true
    flags:(Qt.Window | Qt.CustomizeWindowHint)

    Rectangle{
            z:100
            id:closeButton
            width:15
            height:15
            radius: closeButton.width/2
            anchors.top: parent.top
            anchors.left: parent.left

            Rectangle{
                width:15
                height:15
                radius: 15/2
                anchors.fill: parent
                Text {
                    id: closeName
                    anchors.centerIn: parent
                    text: qsTr("X")
                    color:"#FFFFFF"
                    font.pixelSize: 16
                }
                color:"green"
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onHoveredChanged: cursorShape = Qt.PointingHandCursor
                    onClicked: {
                        groupPage.visible = false
                    }
                }
            }
        }

    ColumnLayout
        {
            z:1
            anchors.fill: parent

            ToolButton
            {
                z:100
                id: reset
                width: 15
                height: 15
                Layout.alignment: Qt.AlignRight

                visible: isOwner

                font.pixelSize :15

                text: setFlag?(reset.pressed? qsTr("<font color='#57c1f2'>设置</font>") :
                                     reset.hovered? qsTr("<font color='#57c1f2'>设置</font>") :
                                                    ("<font color='#e1e7f2'>设置</font>")):
                               (reset.pressed? qsTr("<font color='#57c1f2'>完成</font>") :
                                                                    reset.hovered? qsTr("<font color='#57c1f2'>完成</font>") :
                                                                                   ("<font color='#e1e7f2'>完成</font>"))

                onClicked:
                {
                    if(1 == setFlag)
                    {
                        name.visible = false
                        nameC.visible = true
                        sendMsg.visible = false
                        summaryDisplay.visible = false
                        summaryC.visible = true
                        noticeContext.visible = false
                        noticeC.visible = true
                    }
                    else
                    {
                        name.visible = true
                        nameC.visible = false
                        groupName = nameC.text

                        sendMsg.visible = true

                        summaryDisplay.visible = true
                        summaryC.visible = false
                        groupSummary = summaryC.text

                        noticeContext.visible = true
                        noticeC.visible = false
                        groupNotic = noticeC.text

                        QQClient.changeGInfoRequest(groupName,groupNotic,groupSummary,groupID)
                        console.log("群组信息修改已完成")
                    }

                    setFlag = !setFlag
                }
            }

            Rectangle
            {
                id: img
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: -175
                width: 338
                height: 325
                radius: width/2
                Image
                {

                    anchors.fill: img
                    source: pBackground
                }
                Rectangle
                {
                    id: imgHead
                    width: 100
                    height: 100
                    radius: imgHead.width/2

                    anchors.centerIn: img
                    anchors.verticalCenterOffset: 20

                    Image {
                        id: _image
                        smooth: true
                        anchors.fill: parent
                        visible: false
                        source: groupHead
                        antialiasing: true
                    }
                    Rectangle {
                        id: _mask
                        anchors.fill: parent
                        radius: imgHead.width/2
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
            }

            ColumnLayout
            {
                id: belowInf
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: -120

                Text
                {
                    id: name
                    Layout.alignment: Qt.AlignHCenter
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: 20
                    font.family: "SimHei"
                    font.bold: true
                    text: groupName
                }
                TextInput
                {
                    id: nameC
                    visible: false
                    Layout.alignment: Qt.AlignHCenter
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: 20
                    font.family: "SimHei"
                    font.bold: true
                    text: groupName
                }

                Text
                {
                    id: groupNum
                    Layout.topMargin: 0
                    Layout.alignment: Qt.AlignHCenter
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: 12
                    color: "#767777"
                    text: qsTr("群号：") + groupID
                }
                Button
                {

                    id: sendMsg
                    Layout.alignment: Qt.AlignHCenter
                    Layout.topMargin: -9
                    background:
                    {
                        opacity: 1
                    }
                    icon.source: sendMsg.pressed? "qrc:/images/message1.png":
                                         sendMsg.hovered? "qrc:/images/message1.png" :
                                                        ("qrc:/images/message.png")
                    icon.color:"transparent"
                }
            }

        ColumnLayout
        {
            id:notice
            Layout.topMargin: 0
            Layout.alignment: Qt.AlignHCenter

            Text {
                text: qsTr("<font color='#9f9f9f'>群公告</font>")
            }
            Rectangle
            {
                width: 280
                Text
                {
                    id: noticeContext
                    text: groupNotic
                    font.pixelSize: 13
                }
                TextArea
                {
                    id: noticeC
                    visible: false
                    width : parent.width

                    text: groupNotic
                    font: font.pixelSize = 13
                    background: Rectangle
                    {
                        border.color : "white"
                    }
                }
            }
        }

        Canvas
        {
            id: canvas1
            width: parent.width
            height: parent.height
            onPaint: {
                  var ctx = getContext("2d");
                  draw(ctx);
            }
            function draw(ctx ) {

                ctx.clearRect(0, 0, parent.width, parent.height);

                ctx.fillStyle ="#f2f2f2";
                ctx.strokeStyle = "#f2f2f2";
                ctx.lineWidth = 2

                ctx.beginPath();
                ctx.moveTo(40,340);
                ctx.lineTo(298,340);

                ctx.stroke();
             }
        }

        ColumnLayout
        {
            id:summary
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: 50

            Text {
                text: qsTr("<font color='#9f9f9f'>群介绍</font>")
            }
            Rectangle
            {
                width: 280
                height: 100
                Text
                {
                    id: summaryDisplay
                    text: groupSummary
                }
                TextArea
                {
                    id:summaryC
                    visible: false
                    width: parent.width
                    text: groupSummary
                    font: font.pixelSize = 13
                    background: Rectangle
                    {
                        border.color : "white"
                    }
                }
            }
        }

    }

}

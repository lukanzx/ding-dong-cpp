import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.3

import "./js/time_utils.js" as TimeUtils
import "./components"

Rectangle {
    id: itemContainer
    anchors.left: parent.left
    anchors.right: parent.right
    height: 72
    clip: true
    color: ListView.isCurrentItem ? "#f2f2f2" : "white"
    radius: ListView.isCurrentItem ? 12 : 8

    function cutString(str, len) {

        if(str.length*2 <= len) {
            return str;
        }
        var strlen = 0;
        var s = "";
        for(var i = 0;i < str.length; i++) {
            s = s + str.charAt(i);
            if (str.charCodeAt(i) > 128) {
                strlen = strlen + 2;
                if(strlen >= len){
                    return s.substring(0,s.length-1) + "...";
                }
            } else {
                strlen = strlen + 1;
                if(strlen >= len){
                    return s.substring(0,s.length-2) + "...";
                }
            }
        }
        return s;
    }

    Rectangle {
        id: rectangle
        height: parent.height
        opacity: 1
        width: parent.width
        color: "transparent"

        RoundImage {
            id: avatarImg
            img_src: avatar
            width: 48
            height: avatarImg.width
            anchors.leftMargin: 12
            color: "blue"
            anchors.top: parent.top
            anchors.topMargin: 12
            anchors.left: parent.left
        }

        RowLayout {
            anchors.top: parent.top
            anchors.topMargin: 14
            anchors.left: avatarImg.right
            anchors.right: parent.right

            ColumnLayout {
                id: columnLayout
                Layout.topMargin: 0
                Layout.leftMargin: 12
                Layout.alignment: Qt.AlignLeft
                Label{
                    id: userNameLabel
                    text: userName
                    font.bold: true
                    elide: "ElideRight"
                }
                Label {
                    color: "#ee000000"
                    text: `<font color="#5e5e5e">${cutString(latestMessage, 20)}</font>`
                    elide: Text.ElideRight
                    font.pointSize: 9
                    clip: true
                    width: 200
                }

            }
            Label {
                text: TimeUtils.getTimeString(new Date(latestTime*1000));
                Layout.alignment: Qt.AlignRight | Qt.AlignTop
                Layout.rightMargin: 8
                font.pointSize: 8
            }
        }
    }
}

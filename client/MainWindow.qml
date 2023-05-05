import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.3
import "./components/"

Window {
    id:mainWindowP
    visible: false
    width: 1000
    height: 600
    title: qsTr("WeTalk")
    color: "#edf5f9"
    objectName: "mainWindow"

    signal requestHistoryMessage(int myUid)

    function setHistoryMessage(messages){
        historyMessageScreen.setMessages(messages);
    }

    function startChatTo(uid, name, avatar){
        console.log(`开始和${name}聊天……`)
        historyMessageScreen.startChatWith(uid, name, avatar);
        tabWidget.current = 0
    }

    Component.onCompleted: {
        ClientInfo.id = "100001"
        ClientInfo.name = "Shen"
        ClientInfo.avatar = "https://www.com8.cn/wp-content/uploads/2020/08/20200823052248-5f41fd28d49e4.jpg"
        requestHistoryMessage(ClientInfo.id);
    }

    EmojiChoose{
        id: emojiChoose;
        onClickEmoji: (text)=>chatScreen.appendInputText(text)
    }

    VerticalTabWidget {
        id: tabWidget
        anchors.rightMargin: 12
        anchors.leftMargin: 12
        anchors.bottomMargin: 12
        anchors.topMargin: 12
        anchors.fill: parent
        objectName: "chatWindow1"
        onChangeToContact: contactScreen.sendRequestDataSignal()

        RowLayout {
            id:rowLayout
            spacing: 12
            objectName: "chatWindow2"

            HistoryMessageScreen {
                id: historyMessageScreen
                Layout.fillHeight: true
                width: 270
                objectName: "historyMessageScreen"
                Component.onCompleted: {
                    clickHistoryMessageItem.connect((data)=>{
                        chatScreen.setArg(data["userId"])
                    })
                }
            }

            ChatScreen {
                id: chatScreen
                Layout.fillHeight: true
                onUpdateHistorySignal: (id, message, time)=>historyMessageScreen.setMessageWithUid(id, message, time)
            }
        }

        ContactScreen {
            id: contactScreen
            visible: false
            radius: 4
            onMessageWithConact: (uid, name, avatar)=>startChatTo(uid, name, avatar)
            onMessageWithGroup: (uid, name, avatar)=>startChatTo(uid, name, avatar)
        }

    }
}

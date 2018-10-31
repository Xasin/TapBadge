
import QtQuick 2.11
import QtQuick.Controls 2.2

import QtQuick.Layouts 1.3

import QtQuick.Controls.Material 2.3

ApplicationWindow {
	id: rootWindow

	visible: true
	width: 640
	height: 480
	title: qsTr("Scroll")


	Timer {
		interval: 10000
		onTriggered: tapBadge.startConnecting();

		running: true;
		repeat:  true;
	}

	ScrollView {
		anchors.fill: parent

		ColumnLayout {
			width: rootWindow.width

			RoundButton {
				Layout.fillWidth: true
				Layout.preferredHeight: width;

				onClicked: tapBadge.startConnecting();
			}
		}
	}
}

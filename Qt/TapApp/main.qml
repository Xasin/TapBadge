
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

	ScrollView {
		anchors.fill: parent

		ColumnLayout {
			width: rootWindow.width

			RoundButton {
				Layout.fillWidth: true
				Layout.preferredHeight: width;

				onClicked: {
					reconTimer.restart();
				}
			}

			ProgressBar {
				id: reconTimerBar
				Layout.fillWidth: true

				NumberAnimation on value {
					id: reconTimerBarCountdown
					duration: reconTimer.interval
					easing.type: Easing.Linear
					from: 1
					to:   0
				}

				Connections {
					target: tapBadge.ble_handler

				}
			}
		}
	}
}

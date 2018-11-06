
import QtQuick 2.11
import QtQuick.Controls 2.2

import QtQuick.Layouts 1.3

import QtQuick.Controls.Material 2.3

import com.xasin.tap 1.0

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

			ConnectButton {
				Layout.fillWidth: true
				Layout.preferredHeight: width;
			}

			ProgressBar {
				id: reconTimerBar
				Layout.fillWidth: true

				NumberAnimation on value {
					id: reconTimerBarCountdown
					duration: tapBadge.ble_handler.remainingReconnectTime
					easing.type: Easing.Linear
					from: 1
					to:   0
				}

				Connections {
					target: tapBadge.ble_handler
					onTimerUpdated: {
						reconTimerBarCountdown.restart();
					}
				}
			}
		}
	}
}

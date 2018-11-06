
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

			RoundButton {
				Layout.fillWidth: true
				Layout.preferredHeight: width;

				onClicked: {
					reconTimer.restart();
				}

				background: Rectangle {
					id: reconButtonBackground
					radius: Math.min(width, height);

					anchors.fill: parent;

					Behavior on color {
						ColorAnimation {
							duration: 200
						}
					}

					Connections {
						target: tapBadge.ble_handler
						onConnectionStatusUpdated: {
							var handler = tapBadge.ble_handler;
							var c = handler.connection

							var oColor;
							if(c === BLE_Handler.DISCONNECTED)
								oColor = Material.Red;
							else if(c === BLE_Handler.FINDING)
								oColor = Material.Purple;
							else if(c === BLE_Handler.SYNCHED_PAUSE)
								oColor = Material.Blue;
							else if(c === BLE_Handler.SYNCHED_CONNECTED)
								oColor = Material.Green;
							else if(c === BLE_Handler.SYNCHED_CONNECTING)
								oColor = Material.Cyan;

							if(oColor)
								reconButtonBackground.color = Material.color(oColor);
						}
					}
				}
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

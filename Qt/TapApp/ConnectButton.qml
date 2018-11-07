
import QtQuick 2.11
import QtQuick.Controls 2.2

import QtQuick.Shapes 1.11

import QtQuick.Controls.Material 2.3

import com.xasin.tap 1.0

Item {
	id: rootItem

	Shape {
		anchors.centerIn: parent;

		z: 1

		ShapePath {
			id: countdownPath

			capStyle: ShapePath.RoundCap

			fillColor: "transparent"

			strokeStyle: ShapePath.SolidLine
			strokeWidth: 6
			strokeColor: Material.accent

			startX: 0
			startY: 0

			PathAngleArc {
				id: countdownPathArc
				centerX: 0
				centerY: 0

				radiusX: Math.min(rootItem.width, rootItem.height)/2;
				radiusY: radiusX

				property real fillAngle;

				Behavior on fillAngle {
					SmoothedAnimation {duration: 100}
				}

				startAngle: 90 - fillAngle/2;
				sweepAngle: fillAngle;
			}
		}

		NumberAnimation {
			target: countdownPathArc
			property: "fillAngle"
			id: countdownAnimation
			from: 360
			to: 0
			duration: tapBadge.ble_handler.remainingReconnectTime

			running: true
		}
		Connections {
			target: tapBadge.ble_handler
			onTimerUpdated: countdownAnimation.restart();
		}
	}

	RoundButton {
		id: rootButton

		anchors.fill: parent;

		onClicked: {
			tapBadge.ble_handler.initiate_find();
		}

		background: Rectangle {
			id: reconButtonBackground
			radius: Math.min(width, height);

			anchors.fill: parent;

			Behavior on color {
				ColorAnimation {
					duration: 100
				}
			}

			property int materialCode: Material.Purple;
			color: Material.color(materialCode, rootButton.pressed ? Material.Shade400 : Material.Shade600);
		}

		Connections {
			target: tapBadge.ble_handler
			onConnectionStatusUpdated: {
				var handler = tapBadge.ble_handler;
				var c = handler.connection

				var oColor = -1;
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

				if(oColor != -1)
					reconButtonBackground.materialCode = oColor;
			}
		}
	}
}

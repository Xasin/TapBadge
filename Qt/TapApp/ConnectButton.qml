
import QtQuick 2.11
import QtQuick.Controls 2.2

import QtQuick.Shapes 1.11

import QtQuick.Controls.Material 2.3

import com.xasin.tap 1.0

RoundButton {
	id: rootButton

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

		property int materialCode: Material.Red;
		color: Material.color(materialCode, pressed ? Material.Shade400 : Material.Shade600);

		Shape {
			anchors.fill: parent;

			z: 1

			ShapePath {
				id: countdownPath

				capStyle: ShapePath.RoundCap

				fillColor: "transparent"

				strokeStyle: ShapePath.SolidLine
				strokeWidth: 30
				strokeColor: "white"

				startX: 0
				startY: 0

				PathAngleArc {
					id: countdownPathArc
					centerX: 0
					centerY: 0

					radiusX: rootButton.radius + parent.strokeWidth;
					radiusY: radiusX

					startAngle: 0
					sweepAngle: 360
				}
			}

			NumberAnimation {
				target: countdownPathArc
				property: "sweepAngle"
				id: countdownAnimation
				from: 360
				to: 0
				duration: 5000; //tapBadge.ble_handler.remainingReconnectTime
				loops: Animation.Infinite
			}
			Connections {
				target: tapBadge.ble_handler
				onTimerUpdated: countdownAnimation.restart();
			}
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
				reconButtonBackground.materialCode = oColor;
		}
	}
}

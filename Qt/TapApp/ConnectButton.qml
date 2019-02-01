
import QtQuick 2.11
import QtQuick.Controls 2.2

import QtQuick.Shapes 1.11
import QtGraphicalEffects 1.0

import QtQuick.Controls.Material 2.3

import com.xasin.tap 1.0

Item {
	id: rootItem

	RadialGradient {
		anchors.centerIn: parent;
		width: parent.width + 10
		height: width

		gradient: Gradient {
			GradientStop {
				position: 0.40;
				color: Material.shade(reconButtonBackground.color, Material.Shade600);
			}
			GradientStop {
				position: 0.5
				color: "transparent"
			}
		}
	}

	RoundButton {
		id: rootButton

		anchors.fill: parent;

		onClicked: {
			tapBadge.connHandler.initiate_find("Tap Badge");
		}

		background: Rectangle {
			id: reconButtonBackground
			radius: Math.min(width, height);

			border.color: Material.color(Material.Grey, Material.Shade600);

			anchors.fill: parent;

			Behavior on color {
				ColorAnimation {
					duration: 100
				}
			}

			property int materialCode: Material.Purple;
			color: Material.color(materialCode, rootButton.pressed ? Material.Shade500 : Material.Shade700);
		}

		Connections {
			target: tapBadge.connHandler
			onConnectionStatusUpdated: {
				var handler = tapBadge.connHandler;
				var c = handler.connection

				var oColor = -1;
				if(c === BLE_Handler.DISCONNECTED)
					oColor = Material.Red;
				else if(c === BLE_Handler.FINDING)
					oColor = Material.Purple;
				else if(c === BLE_Handler.RECONNECTING)
					oColor = Material.Cyan;
				else if(c === BLE_Handler.CONNECTED)
					oColor = Material.Green;

				if(oColor != -1)
					reconButtonBackground.materialCode = oColor;
			}
		}
	}
}

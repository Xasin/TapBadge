
import QtQuick 2.11
import QtQuick.Controls 2.2

import QtQuick.Controls.Material 2.3

import com.xasin.tap 1.0

RoundButton {
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

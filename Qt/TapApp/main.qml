
import QtQuick 2.11
import QtQuick.Controls 2.2

import QtQuick.Layouts 1.3

import QtApplicationManager 1.0

import QtQuick.Controls.Material 2.3

import com.xasin.tap 1.0

ApplicationWindow {
	id: rootWindow

	visible: true
	width: 640
	height: 480
	title: qsTr("Scroll")

	Material.accent: [Material.Grey, Material.Red, Material.Blue, Material.Green][tapBadge.whoIs]

	Notification {
		priority: 0

		progress: -1
		showProgress: true
		sticky: true

		visible: true
	}

	ScrollView {
		anchors.fill: parent

		ColumnLayout {
			width: rootWindow.width

			ConnectButton {
				Layout.fillWidth: true
				Layout.preferredHeight: width;

				Layout.margins: 30;
			}

			Rectangle {
				Layout.fillWidth: true
				Layout.preferredHeight: 50;
				Layout.margins: 10

				radius: 5

				clip: true

				color: "transparent"
				border.color: Material.shade(Material.accent, Material.Shade900);
				border.width: 2

				TabBar {
					anchors.fill: parent;
					anchors.margins: 2

					currentIndex: tapBadge.whoIs -1;

					TabButton {
						text: "Xasin"
						onPressed: tapBadge.whoIs = 1
					}
					TabButton {
						text: "Neira"
						onPressed: tapBadge.whoIs = 2
					}
					TabButton {
						text: "Mesh"
						onPressed: tapBadge.whoIs = 3
					}
				}
			}

			Pane {
				Layout.margins: 10;
				Layout.fillWidth: true;
				Layout.preferredHeight: 30;
				Material.elevation: 3

				padding: 0

				Label {
					anchors.fill: parent;
					text: "Power: " + tapBadge.batteryMV + "mV";

					horizontalAlignment:  Text.AlignHCenter;
				}

				ProgressBar {
					value: tapBadge.batteryPercent / 100;
					anchors.bottom: parent.bottom
					width: parent.width

					Material.accent: Material.Blue
				}
			}
		}
	}

	Component.onCompleted: {
		tapBadge.ble_handler.initiate_find();
	}
}

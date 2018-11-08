
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

	Material.accent: [Material.Grey, Material.Red, Material.Blue, Material.Green][tapBadge.whoIs]

	ScrollView {
		anchors.fill: parent

		ColumnLayout {
			width: rootWindow.width

			ConnectButton {
				Layout.fillWidth: true
				Layout.preferredHeight: width;

				Layout.margins: 30;
				Layout.leftMargin: 50
				Layout.rightMargin: 50
			}

			Frame {
				Layout.fillWidth: true
				Layout.preferredHeight: 50;
				Layout.margins: 10

				Material.elevation: 3;

				padding: 2

				TabBar {
					anchors.fill: parent;
					anchors.margins: 0

					height: parent.height - parent.padding*2

					currentIndex: tapBadge.whoIs;

					TabButton {
						text: "Sleep"
						onPressed: tapBadge.whoIs = 0;
					}
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

			Frame {
				Layout.margins: 10;
				Layout.fillWidth: true;
				Layout.preferredHeight: 36;
				Material.elevation: 3

				padding: 1

				Label {
					anchors.fill: parent;
					text: "Power: " + tapBadge.batteryMV + "mV";

					horizontalAlignment:  Text.AlignHCenter;
					verticalAlignment:    Text.AlignVCenter;

					font.pixelSize: 18
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

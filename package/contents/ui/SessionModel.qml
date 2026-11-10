pragma ComponentBehavior: Bound
pragma ValueTypeBehavior: Addressable

import QtQuick
import org.kde.plasma.workspace.dbus as DBus

// Tracks whether this user's login session is the active one (e.g. not switched to another user)
Item {
    id: root
    property bool sessionIsActive: true

    property DBus.dbusMessage msg: {
        "service": "org.freedesktop.login1",
        "path": "/org/freedesktop/login1/session/auto",
        "iface": "org.freedesktop.DBus.Properties",
        "member": "Get",
        "arguments": ["org.freedesktop.login1.Session", "Active"],
        "signature": null,
        "inSignature": "ss"
    }

    function update() {
        const reply = DBus.SystemBus.asyncCall(root.msg) as DBus.DBusPendingReply;
        reply.finished.connect(() => {
            // Get returns a variant, which may arrive nested
            const active = typeof reply.value === "object" ? reply.value?.value : reply.value;
            if (typeof active === "boolean") {
                root.sessionIsActive = active;
            }
            reply.destroy();
        });
    }

    Timer {
        running: true
        repeat: true
        triggeredOnStart: true
        interval: 1000
        onTriggered: root.update()
    }
}

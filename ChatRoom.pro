#-------------------------------------------------
#
# Project created by QtCreator 2016-07-30T13:43:43
#
#-------------------------------------------------

CONFIG += c++11
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ChatRoom
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    server.cpp \
    client.cpp \
    message.cpp \
    error.cpp \
    chat.cpp \
    my_time.cpp \
    chat_form.cpp \
    change_nickname_dialog.cpp \
    error_message_form.cpp \
    tcp_socket.cpp

HEADERS  += mainwindow.h \
    server.h \
    client.h \
    message.h \
    error.h \
    chat.h \
    my_time.h \
    chat_form.h \
    change_nickname_dialog.h \
    error_message_form.h \
    tcp_socket.h

FORMS    += mainwindow.ui \
    chat_form.ui \
    change_nickname_dialog.ui \
    error_message_form.ui

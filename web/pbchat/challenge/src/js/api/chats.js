
import db from '../db/firestore';
import firebase from 'firebase/app';

const extractSnapshotData = snapshot =>
  snapshot.docs.map(doc => ({id: doc.id, ...doc.data()}))

export const fetchChats = (room) =>
  db
    .collection(`rooms/${room}/chats`)
    .get()
    .then(extractSnapshotData)

export const createChat = (room, chat) =>
  db
    .collection(`rooms/${room}/chats`)
    .add(chat)
    .then(docRef => docRef.id)

export const joinChat = async (room, userId, chatId) => {
  const userRef = db.doc(`rooms/${room}/profiles/${userId}`);
  const chatRef = db.doc(`rooms/${room}/chats/${chatId}`);
  
  await userRef.update({joinedChats: firebase.firestore.FieldValue.arrayUnion(chatRef)})
  await chatRef.update({joinedUsers: firebase.firestore.FieldValue.arrayUnion(userRef)})
}

export const subscribeToChat = (room, chatId, onSubsribe) =>
  db
    .collection(`rooms/${room}/chats`)
    .doc(chatId)
    .onSnapshot(snapshot => {
      const chat = {id: snapshot.id, ...snapshot.data()}
      onSubsribe(chat);
    })

export const subscribeToProfile = (room, uid, onSubsribe) =>
  db
    .collection(`rooms/${room}/profiles`)
    .doc(uid)
    .onSnapshot(snapshot => onSubsribe(snapshot.data()))

export const sendChatMessage = (room, message, chatId) =>
  db
    .collection(`rooms/${room}/chats`)
    .doc(chatId)
    .collection('messages')
    .doc(message.timestamp)
    .set(message)

export const subscribeToMessages = (room, chatId, onSubscribe) =>
  db
    .collection(`rooms/${room}/chats`)
    .doc(chatId)
    .collection('messages')
    .onSnapshot(snapshot => onSubscribe(snapshot.docChanges()))

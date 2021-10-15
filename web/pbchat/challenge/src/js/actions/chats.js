import * as api from '../api/chats';
import db from '../db/firestore';

export const fetchChats = () => async (dispatch, getState) => {
  const { user } = getState().auth;
  dispatch({ type: 'CHATS_FETCH_INIT' });
  const chats = await api.fetchChats(user.room);

  chats
    .forEach(chat => chat.joinedUsers = chat.joinedUsers.map(user => user.id));
  const sortedChats = chats.reduce((accuChats, chat) => {
    accuChats[chat.joinedUsers.includes(user.uid) ? 'joined' : 'available'].push(chat);
    return accuChats;
  }, { joined: [], available: [] })

  dispatch({
    type: 'CHATS_FETCH_SUCCESS',
    ...sortedChats
  })

  return sortedChats;
}

export const joinChat = (chat, uid) => (dispatch, getState) => {
  const { user } = getState().auth;
  api.joinChat(user.room, uid, chat.id)
    .then(_ => {
      dispatch({ type: 'CHATS_JOIN_SUCCESS', chat });
    })
}

export const createChat = (formData, userId) => async (dispatch, getState) => {
  const newChat = { ...formData };
  const { user } = getState().auth;
  newChat.admin = db.doc(`rooms/${user.room}/profiles/${userId}`);

  const chatId = await api.createChat(user.room, newChat);
  dispatch({ type: 'CHATS_CREATE_SUCCESS' });
  await api.joinChat(user.room, userId, chatId)
  dispatch({ type: 'CHATS_JOIN_SUCCESS', chat: { ...newChat, id: chatId } });
  return chatId;
}

export const subscribeToChat = chatId => (dispatch, getState) => {
  const { user } = getState().auth;
  api
    .subscribeToChat(user.room, chatId, async (chat) => {
      const joinedUsers = await Promise.all(chat.joinedUsers.map(async userRef => {
        const userSnapshot = await userRef.get();
        return userSnapshot.data();
      }))

      chat.joinedUsers = joinedUsers;
      dispatch({ type: 'CHATS_SET_ACTIVE_CHAT', chat })
    })
}

export const subscribeToProfile = (uid, chatId) => (dispatch, getState) => {
  const { user } = getState().auth;
  api
    .subscribeToProfile(user.room, uid, user => {
      dispatch({ type: 'CHATS_UPDATE_USER_STATE', user, chatId })
    })
}

export const sendChatMessage = (message, chatId) => (dispatch, getState) => {
  const newMessage = { ...message };
  const { user } = getState().auth;
  const userRef = db.doc(`rooms/${user.room}/profiles/${user.uid}`);
  newMessage.author = userRef;

  return api
    .sendChatMessage(user.room, newMessage, chatId)
    .then(_ => dispatch({ type: 'CHATS_MESSAGE_SENT' }))
}

export const subscribeToMessages = chatId => (dispatch, getState) => {
  const { user } = getState().auth;
  return api.subscribeToMessages(user.room, chatId, async changes => {
    const messages = changes.map(change => {
      if (change.type === 'added') {
        return { id: change.doc.id, ...change.doc.data() }
      }
    })

    const messagesWithAuthor = [];
    const cache = {}

    for await (let message of messages) {
      if (cache[message.author.id]) {
        message.author = cache[message.author.id]
      } else {
        const userSnapshot = await message.author.get();
        cache[userSnapshot.id] = userSnapshot.data();
        message.author = cache[userSnapshot.id]
      }

      messagesWithAuthor.push(message);
    }

    return dispatch({ type: 'CHATS_SET_MESSAGES', messages: messagesWithAuthor, chatId })
  })
}

export const registerMessageSubscription = (chatId, messageSub) => ({
  type: 'CHATS_REGISTER_MESSAGE_SUB',
  sub: messageSub,
  chatId
})

import React from 'react';
import ChatSearch from './ChatSearch';

export default function JoinedChatsList({chats}) {
  return (
    <div className="list-container">
      <ChatSearch />
      <ul className="items">
        {
          chats.map(chat =>
            <li
              key={chat.id}
              onClick={() => window.open(`./index.html#/chat/${chat.id}`, `chat-${chat.id}`)}
              className="item">
              <div className="item-status">
                <img src={chat.image} alt="" />
                <span className="status online"></span>
              </div>
              <p className="name-time">
                <span className="name mr-2">{chat.name}</span>
              </p>
            </li>
          )
        }
      </ul>
    </div>
  )
}



import React, { useCallback } from 'react';
import { useSelector } from 'react-redux';
import { formatTimeAgo } from '../utils/time';
import Linkify from './Linkify';

export default function ChatMessagesList({messages = [], innerRef}) {
  const user = useSelector(({auth}) => auth.user);

  const isAuthorOf = useCallback(message => {
    return message?.author.uid === user.uid ? 'chat-right' : 'chat-left';
  })

  return (
    <div className="chat-container">
      <ul ref={innerRef} className="chat-box chatContainerScroll">
        { messages.map(message =>
          <li
            key={message.id}
            className={isAuthorOf(message)}>
            <div className="chat-avatar">
              <img
                src={message?.author.avatar || "https://img.icons8.com/material-rounded/24/000000/user.png"}
                alt="" />
              <div className="chat-name">{message?.author.username}</div>
            </div>
            <div className="chat-text-wrapper">
              <span className="chat-text">
                {
                  message.isPalantirEnabled
                  ? <Linkify text={message.content} />
                  : message.content
                }
              </span>
              <span className="chat-spacer"></span>
              <div className="chat-hour">{formatTimeAgo(message.timestamp)}</div>
            </div>
          </li>
        )}
      </ul>
    </div>
  )
}

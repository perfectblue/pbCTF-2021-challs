

import React from 'react';


export default function ChatUserList({users = []}) {
  return(
    <div className="list-container">
      <div className="chat-search-box">
        <div className="input-group">
          <input className="form-control" placeholder="Search" />
        </div>
      </div>
      <ul className="items">
        { users.map(user => (
           <li
            key={user.uid}
            className="item">
            <div className="item-status">
              <img
                src={user.avatar || "https://img.icons8.com/material-rounded/24/000000/user.png"}
                alt="Young Hobbit" />
              <span className={`status ${user.state}`}></span>
            </div>
            <p className="name-time">
            <span className="name mr-2">{user.username}</span>
            </p>
          </li>
        ))
        }
      </ul>
    </div>
  )
}

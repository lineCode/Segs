/*
 * SEGS - Super Entity Game Server
 * http://www.segs.io/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#include "EmailService.h"
#include "Messages/EmailService/EmailDefinitions.h"
#include "Messages/EmailService/EmailEvents.h"
#include "Messages/Map/EmailHeaders.h"
#include "Messages/Map/EmailMessageStatus.h"
#include "Messages/Map/EmailRead.h"
#include "Common/Servers/InternalEvents.h"
#include "Servers/MapServer/MessageHelpers.h"

using namespace SEGSEvents;

ServiceToClientData* EmailService::on_email_header_response(Event* ev)
{
    EmailHeaderResponse* casted_ev = static_cast<EmailHeaderResponse *>(ev);

    std::vector<EmailHeaders::EmailHeader> emailHeadersVector;
    emailHeadersVector.reserve(casted_ev->m_data.m_email_headers.size());
    for (const auto &email_header : casted_ev->m_data.m_email_headers)
    {
        emailHeadersVector.push_back(EmailHeaders::EmailHeader{email_header.m_email_id,
                                                          email_header.m_sender_name,
                                                          email_header.m_subject,
                                                          email_header.m_timestamp});
    }

    std::unique_ptr<EmailHeaders> emailHeaders = std::make_unique<EmailHeaders>(emailHeadersVector);
    GameCommandVector commands;
    commands.push_back(std::move(emailHeaders));

    return new ServiceToClientData(casted_ev->session_token(), std::move(commands), QString());
}

// EmailHandler will send this event here
ServiceToClientData* EmailService::on_email_header_to_client(Event* ev)
{
    EmailHeaderToClientMessage* casted_ev = static_cast<EmailHeaderToClientMessage *>(ev);
    std::unique_ptr<EmailHeaders> emailHeaders = std::make_unique<EmailHeaders>( casted_ev->m_data.m_email_id, casted_ev->m_data.m_sender_name,
                                                                                 casted_ev->m_data.m_subject, casted_ev->m_data.m_timestamp);
    GameCommandVector commands;
    commands.push_back(std::move(emailHeaders));

    return new ServiceToClientData(casted_ev->session_token(), std::move(commands), QString());
}

ServiceToClientData* EmailService::on_email_headers_to_client(Event* ev)
{
    EmailHeadersToClientMessage* casted_ev = static_cast<EmailHeadersToClientMessage *>(ev);

    std::vector<EmailHeaders::EmailHeader> emailHeadersVector;
    emailHeadersVector.reserve(casted_ev->m_data.m_email_headers.size());
    for (const auto &email_header : casted_ev->m_data.m_email_headers)
    {
        emailHeadersVector.push_back(EmailHeaders::EmailHeader{email_header.m_email_id,
                                                          email_header.m_sender_name,
                                                          email_header.m_subject,
                                                          email_header.m_timestamp});
    }

    std::unique_ptr<EmailHeaders> emailHeaders = std::make_unique<EmailHeaders>(emailHeadersVector);
    GameCommandVector commands;
    commands.push_back(std::move(emailHeaders));

    QString messageToClient = QString("You have %1 unread emails.").arg(casted_ev->m_data.m_unread_emails_count);

    return new ServiceToClientData(casted_ev->session_token(), std::move(commands), messageToClient);
}

ServiceToClientData* EmailService::on_email_read_response(Event* ev)
{
    EmailReadResponse* casted_ev = static_cast<EmailReadResponse *>(ev);
    std::unique_ptr<EmailRead> emailRead = std::make_unique<EmailRead>(
                casted_ev->m_data.m_email_id, casted_ev->m_data.m_message, casted_ev->m_data.m_sender_name);
    GameCommandVector commands;
    commands.push_back(std::move(emailRead));

    return new ServiceToClientData(casted_ev->session_token(), std::move(commands), QString());
}

ServiceToClientData* EmailService::on_email_read_by_recipient(Event* ev)
{
    EmailWasReadByRecipientMessage* casted_ev = static_cast<EmailWasReadByRecipientMessage *>(ev);
    return new ServiceToClientData(casted_ev->session_token(), {}, casted_ev->m_data.m_message);
}

ServiceToClientData* EmailService::on_email_create_status(Event* ev)
{
    EmailCreateStatusMessage* casted_ev = static_cast<EmailCreateStatusMessage* >(ev);

    std::unique_ptr<EmailMessageStatus> emailStatus = std::make_unique<EmailMessageStatus>(casted_ev->m_data.m_status, casted_ev->m_data.m_recipient_name);
    GameCommandVector commands;
    commands.push_back(std::move(emailStatus));

    return new ServiceToClientData(casted_ev->session_token(), std::move(commands), QString());
}

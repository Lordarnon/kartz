/*
 * This file is part of the Marmalade SDK Code Samples.
 *
 * (C) 2001-2012 Marmalade. All Rights Reserved.
 *
 * This source code is intended only as a supplement to the Marmalade SDK.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */
#ifndef INCARCAMERA_H
#define INCARCAMERA_H

#include "IwBehaviour.h"

class IwEnt;

class CInCarCamera : public CIwBehaviour
{
public:
    CInCarCamera(CIwEnt* target);

    virtual void Update();

private:
    CIwEnt* m_Target;
};

#endif /* !INCARCAMERA_H */
